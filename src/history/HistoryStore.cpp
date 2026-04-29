#include "history/HistoryStore.h"

#include <windows.h>

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

namespace cyberdeck::history {
namespace {

struct JsonValue {
    using Array = std::vector<JsonValue>;
    using Object = std::map<std::string, JsonValue>;

    std::variant<std::nullptr_t, bool, double, std::string, Array, Object> value;
};

class JsonParser {
public:
    explicit JsonParser(std::string_view input) : input_(input) {}

    std::optional<JsonValue> Parse() {
        auto value = ParseValue();
        if (!value) {
            return std::nullopt;
        }

        SkipWhitespace();
        if (position_ != input_.size()) {
            return std::nullopt;
        }

        return value;
    }

    std::size_t position() const {
        return position_;
    }

private:
    std::optional<JsonValue> ParseValue() {
        SkipWhitespace();
        if (position_ >= input_.size()) {
            return std::nullopt;
        }

        const char current = input_[position_];
        if (current == '"') {
            auto string = ParseString();
            if (!string) {
                return std::nullopt;
            }
            return JsonValue{*string};
        }
        if (current == '{') {
            return ParseObject();
        }
        if (current == '[') {
            return ParseArray();
        }
        if (std::isdigit(static_cast<unsigned char>(current)) || current == '-') {
            return ParseNumber();
        }
        if (ConsumeLiteral("true")) {
            return JsonValue{true};
        }
        if (ConsumeLiteral("false")) {
            return JsonValue{false};
        }
        if (ConsumeLiteral("null")) {
            return JsonValue{nullptr};
        }

        return std::nullopt;
    }

    std::optional<JsonValue> ParseObject() {
        if (!Consume('{')) {
            return std::nullopt;
        }

        JsonValue::Object object;
        SkipWhitespace();
        if (Consume('}')) {
            return JsonValue{std::move(object)};
        }

        while (true) {
            SkipWhitespace();
            auto key = ParseString();
            if (!key) {
                return std::nullopt;
            }
            if (!Consume(':')) {
                return std::nullopt;
            }
            auto value = ParseValue();
            if (!value) {
                return std::nullopt;
            }
            object.emplace(std::move(*key), std::move(*value));

            SkipWhitespace();
            if (Consume('}')) {
                break;
            }
            if (!Consume(',')) {
                return std::nullopt;
            }
        }

        return JsonValue{std::move(object)};
    }

    std::optional<JsonValue> ParseArray() {
        if (!Consume('[')) {
            return std::nullopt;
        }

        JsonValue::Array array;
        SkipWhitespace();
        if (Consume(']')) {
            return JsonValue{std::move(array)};
        }

        while (true) {
            SkipWhitespace();
            auto value = ParseValue();
            if (!value) {
                return std::nullopt;
            }
            array.push_back(std::move(*value));

            SkipWhitespace();
            if (Consume(']')) {
                break;
            }
            if (!Consume(',')) {
                return std::nullopt;
            }
        }

        return JsonValue{std::move(array)};
    }

    std::optional<std::string> ParseString() {
        if (!Consume('"')) {
            return std::nullopt;
        }

        std::string output;
        while (position_ < input_.size()) {
            const char current = input_[position_++];
            if (current == '"') {
                return output;
            }
            if (static_cast<unsigned char>(current) < 0x20) {
                return std::nullopt;
            }
            if (current != '\\') {
                output.push_back(current);
                continue;
            }
            if (position_ >= input_.size()) {
                return std::nullopt;
            }

            const char escaped = input_[position_++];
            switch (escaped) {
                case '"':
                case '\\':
                case '/':
                    output.push_back(escaped);
                    break;
                case 'b':
                    output.push_back('\b');
                    break;
                case 'f':
                    output.push_back('\f');
                    break;
                case 'n':
                    output.push_back('\n');
                    break;
                case 'r':
                    output.push_back('\r');
                    break;
                case 't':
                    output.push_back('\t');
                    break;
                case 'u':
                    if (!AppendUnicodeEscape(output)) {
                        return std::nullopt;
                    }
                    break;
                default:
                    return std::nullopt;
            }
        }

        return std::nullopt;
    }

    std::optional<JsonValue> ParseNumber() {
        const std::size_t start = position_;
        if (input_[position_] == '-') {
            ++position_;
        }
        if (position_ >= input_.size() || !std::isdigit(static_cast<unsigned char>(input_[position_]))) {
            return std::nullopt;
        }
        while (position_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[position_]))) {
            ++position_;
        }

        if (position_ < input_.size() && input_[position_] == '.') {
            ++position_;
            if (position_ >= input_.size() || !std::isdigit(static_cast<unsigned char>(input_[position_]))) {
                return std::nullopt;
            }
            while (position_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[position_]))) {
                ++position_;
            }
        }

        try {
            return JsonValue{std::stod(std::string(input_.substr(start, position_ - start)))};
        } catch (...) {
            return std::nullopt;
        }
    }

    bool AppendUnicodeEscape(std::string& output) {
        if (position_ + 4 > input_.size()) {
            return false;
        }

        int codepoint = 0;
        for (int index = 0; index < 4; ++index) {
            const char ch = input_[position_++];
            codepoint <<= 4;
            if (ch >= '0' && ch <= '9') {
                codepoint += ch - '0';
            } else if (ch >= 'a' && ch <= 'f') {
                codepoint += ch - 'a' + 10;
            } else if (ch >= 'A' && ch <= 'F') {
                codepoint += ch - 'A' + 10;
            } else {
                return false;
            }
        }

        if (codepoint <= 0x7F) {
            output.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7FF) {
            output.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
            output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else {
            output.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
            output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }

        return true;
    }

    bool Consume(char expected) {
        SkipWhitespace();
        if (position_ >= input_.size() || input_[position_] != expected) {
            return false;
        }
        ++position_;
        return true;
    }

    bool ConsumeLiteral(std::string_view literal) {
        if (input_.substr(position_, literal.size()) != literal) {
            return false;
        }
        position_ += literal.size();
        return true;
    }

    void SkipWhitespace() {
        while (position_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[position_]))) {
            ++position_;
        }
    }

    std::string_view input_;
    std::size_t position_ = 0;
};

std::string CurrentUtcTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm utc_time{};
#if defined(_MSC_VER)
    gmtime_s(&utc_time, &time);
#else
    gmtime_s(&utc_time, &time);
#endif

    std::ostringstream output;
    output << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

std::string CurrentFileTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};
#if defined(_MSC_VER)
    localtime_s(&local_time, &time);
#else
    localtime_s(&local_time, &time);
#endif

    std::ostringstream output;
    output << std::put_time(&local_time, "%Y%m%d%H%M%S");
    return output.str();
}

std::string NarrowForLog(const std::filesystem::path& path) {
    const std::wstring wide = path.wstring();
    std::string output;
    output.reserve(wide.size());
    for (wchar_t ch : wide) {
        output.push_back(ch >= 32 && ch <= 126 ? static_cast<char>(ch) : '?');
    }
    return output;
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return {};
    }

    const int required = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        value.data(),
        static_cast<int>(value.size()),
        nullptr,
        0,
        nullptr,
        nullptr);
    if (required <= 0) {
        return {};
    }

    std::string output(static_cast<std::size_t>(required), '\0');
    WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        value.data(),
        static_cast<int>(value.size()),
        output.data(),
        required,
        nullptr,
        nullptr);
    return output;
}

std::wstring Utf8ToWide(const std::string& value) {
    if (value.empty()) {
        return {};
    }

    const int required = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        value.data(),
        static_cast<int>(value.size()),
        nullptr,
        0);
    if (required <= 0) {
        return {};
    }

    std::wstring output(static_cast<std::size_t>(required), L'\0');
    MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        value.data(),
        static_cast<int>(value.size()),
        output.data(),
        required);
    return output;
}

std::string EscapeJsonString(const std::wstring& value) {
    std::string output;
    for (unsigned char ch : WideToUtf8(value)) {
        switch (ch) {
            case '"':
                output += "\\\"";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            default:
                if (ch < 0x20) {
                    output += "\\u00";
                    constexpr char hex[] = "0123456789ABCDEF";
                    output.push_back(hex[(ch >> 4) & 0x0F]);
                    output.push_back(hex[ch & 0x0F]);
                } else {
                    output.push_back(static_cast<char>(ch));
                }
                break;
        }
    }
    return output;
}

std::string EscapeJsonAscii(std::string_view value) {
    std::string output;
    for (char ch : value) {
        if (ch == '"' || ch == '\\') {
            output.push_back('\\');
        }
        output.push_back(ch);
    }
    return output;
}

const JsonValue::Object* AsObject(const JsonValue& value) {
    return std::get_if<JsonValue::Object>(&value.value);
}

const JsonValue::Array* AsArray(const JsonValue& value) {
    return std::get_if<JsonValue::Array>(&value.value);
}

const std::string* StringField(const JsonValue::Object& object, const std::string& key) {
    const auto found = object.find(key);
    if (found == object.end()) {
        return nullptr;
    }
    return std::get_if<std::string>(&found->second.value);
}

std::optional<int> IntField(const JsonValue::Object& object, const std::string& key) {
    const auto found = object.find(key);
    if (found == object.end()) {
        return std::nullopt;
    }

    const double* value = std::get_if<double>(&found->second.value);
    if (value == nullptr || *value < 0 || *value > static_cast<double>(INT_MAX)) {
        return std::nullopt;
    }

    const int result = static_cast<int>(*value);
    if (static_cast<double>(result) != *value) {
        return std::nullopt;
    }

    return result;
}

bool IsUsableEntry(const HistoryEntry& entry) {
    return entry.id > 0 && !entry.url.empty() && entry.visit_count > 0 &&
           !entry.first_visited_utc.empty() && !entry.last_visited_utc.empty();
}

}  // namespace

std::filesystem::path HistoryStore::DefaultHistoryPath() {
    DWORD required = GetEnvironmentVariableW(L"APPDATA", nullptr, 0);
    std::filesystem::path root;
    if (required > 0) {
        std::wstring app_data(required, L'\0');
        const DWORD copied = GetEnvironmentVariableW(L"APPDATA", app_data.data(), required);
        if (copied > 0) {
            app_data.resize(copied);
            root = app_data;
        }
    }

    if (root.empty()) {
        root = std::filesystem::current_path() / "dev" / "appdata";
    }

    return root / "CyberDeckBrowser" / "history.json";
}

bool HistoryStore::Initialize(std::filesystem::path history_path, common::Logger& logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_ = &logger;
    history_path_ = std::move(history_path);
    entries_.clear();
    next_id_ = 1;

    if (!LoadLocked()) {
        if (!RenameCorruptedFileLocked()) {
            logger_->Error("Unable to recover corrupted history file.");
            return false;
        }
        entries_.clear();
        next_id_ = 1;
        if (!WriteLocked()) {
            logger_->Error("Unable to write clean history file after corruption recovery.");
            return false;
        }
    }

    logger_->Info("History store ready: " + NarrowForLog(history_path_));
    return true;
}

bool HistoryStore::RecordVisit(const std::wstring& title, const std::wstring& url) {
    if (url.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (history_path_.empty() || logger_ == nullptr) {
        return false;
    }

    const std::string now = CurrentUtcTimestamp();
    auto found = std::find_if(entries_.begin(), entries_.end(), [&url](const HistoryEntry& entry) {
        return entry.url == url;
    });

    if (found == entries_.end()) {
        HistoryEntry entry;
        entry.id = next_id_++;
        entry.title = title.empty() ? url : title;
        entry.url = url;
        entry.visit_count = 1;
        entry.first_visited_utc = now;
        entry.last_visited_utc = now;
        entries_.push_back(std::move(entry));
    } else {
        if (!title.empty()) {
            found->title = title;
        }
        ++found->visit_count;
        found->last_visited_utc = now;
    }

    const bool written = WriteLocked();
    if (!written) {
        logger_->Error("Failed to write history entry.");
    }
    return written;
}

std::vector<HistoryEntry> HistoryStore::Entries() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_;
}

std::filesystem::path HistoryStore::path() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return history_path_;
}

bool HistoryStore::LoadLocked() {
    auto fail = [this](std::string_view reason) {
        if (logger_ != nullptr) {
            logger_->Error(std::string("History file invalid: ") + std::string(reason));
        }
        return false;
    };

    if (history_path_.empty()) {
        return fail("empty path");
    }

    std::error_code error;
    if (!std::filesystem::exists(history_path_, error)) {
        return WriteLocked();
    }

    std::ifstream input(history_path_, std::ios::binary);
    if (!input.is_open()) {
        return fail("could not open file");
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    const std::string json_text = buffer.str();
    JsonParser parser(json_text);
    const auto root_value = parser.Parse();
    if (!root_value) {
        if (logger_ != nullptr) {
            logger_->Error("History parse stopped near byte " + std::to_string(parser.position()) + ".");
        }
        return fail("JSON parse failed");
    }

    const JsonValue::Object* root = AsObject(*root_value);
    if (root == nullptr) {
        return fail("root is not an object");
    }

    const auto entries_found = root->find("entries");
    if (entries_found == root->end()) {
        return fail("entries field is missing");
    }

    const JsonValue::Array* entries = AsArray(entries_found->second);
    if (entries == nullptr) {
        return fail("entries field is not an array");
    }

    std::vector<HistoryEntry> loaded;
    int maximum_id = 0;
    for (const JsonValue& item : *entries) {
        const JsonValue::Object* object = AsObject(item);
        if (object == nullptr) {
            return fail("entry is not an object");
        }

        HistoryEntry entry;
        const auto id = IntField(*object, "id");
        const auto visit_count = IntField(*object, "visitCount");
        const std::string* title = StringField(*object, "title");
        const std::string* url = StringField(*object, "url");
        const std::string* first_visited = StringField(*object, "firstVisitedUtc");
        const std::string* last_visited = StringField(*object, "lastVisitedUtc");
        if (!id || !visit_count || title == nullptr || url == nullptr ||
            first_visited == nullptr || last_visited == nullptr) {
            return fail("entry has missing or invalid fields");
        }

        entry.id = *id;
        entry.title = Utf8ToWide(*title);
        entry.url = Utf8ToWide(*url);
        entry.visit_count = *visit_count;
        entry.first_visited_utc = *first_visited;
        entry.last_visited_utc = *last_visited;
        if (!IsUsableEntry(entry)) {
            return fail("entry has unusable values");
        }

        maximum_id = std::max(maximum_id, entry.id);
        loaded.push_back(std::move(entry));
    }

    entries_ = std::move(loaded);
    next_id_ = maximum_id + 1;
    return true;
}

bool HistoryStore::WriteLocked() {
    std::error_code error;
    std::filesystem::create_directories(history_path_.parent_path(), error);
    if (error) {
        return false;
    }

    const std::filesystem::path temp_path = history_path_.string() + ".tmp";
    {
        std::ofstream output(temp_path, std::ios::binary | std::ios::trunc);
        if (!output.is_open()) {
            return false;
        }

        output << "{\n";
        output << "  \"version\": 1,\n";
        output << "  \"entries\": [\n";
        for (std::size_t index = 0; index < entries_.size(); ++index) {
            const HistoryEntry& entry = entries_[index];
            output << "    {\n";
            output << "      \"id\": " << entry.id << ",\n";
            output << "      \"title\": \"" << EscapeJsonString(entry.title) << "\",\n";
            output << "      \"url\": \"" << EscapeJsonString(entry.url) << "\",\n";
            output << "      \"visitCount\": " << entry.visit_count << ",\n";
            output << "      \"firstVisitedUtc\": \"" << EscapeJsonAscii(entry.first_visited_utc) << "\",\n";
            output << "      \"lastVisitedUtc\": \"" << EscapeJsonAscii(entry.last_visited_utc) << "\"\n";
            output << "    }" << (index + 1 == entries_.size() ? "\n" : ",\n");
        }
        output << "  ]\n";
        output << "}\n";
        output.flush();
        if (!output.good()) {
            return false;
        }
    }

    if (!MoveFileExW(temp_path.c_str(), history_path_.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        std::filesystem::remove(temp_path, error);
        return false;
    }

    return true;
}

bool HistoryStore::RenameCorruptedFileLocked() {
    std::error_code error;
    if (!std::filesystem::exists(history_path_, error)) {
        return true;
    }

    const std::filesystem::path corrupted_path =
        history_path_.parent_path() /
        (history_path_.filename().wstring() + L".corrupt." + Utf8ToWide(CurrentFileTimestamp()) + L".bak");

    if (!MoveFileExW(history_path_.c_str(), corrupted_path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        return false;
    }

    if (logger_ != nullptr) {
        logger_->Error("Corrupted history file renamed to: " + NarrowForLog(corrupted_path));
    }
    return true;
}

}  // namespace cyberdeck::history
