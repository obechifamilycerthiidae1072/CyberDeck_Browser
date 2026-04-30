#include "settings/SettingsStore.h"

#include "common/Platform.h"

#include <algorithm>
#include <chrono>
#include <climits>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string_view>
#include <system_error>
#include <utility>

namespace cyberdeck::settings {
namespace {

std::string WideToUtf8(const std::wstring& value) {
    return common::WideToUtf8(value);
}

std::wstring Utf8ToWide(const std::string& value) {
    return common::Utf8ToWide(value);
}

std::string NarrowForLog(const std::filesystem::path& path) {
    return WideToUtf8(path.wstring());
}

std::string CurrentFileTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};
#if defined(_MSC_VER)
    localtime_s(&local_time, &time);
#elif defined(__MINGW32__)
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    std::ostringstream output;
    output << std::put_time(&local_time, "%Y%m%d-%H%M%S");
    return output.str();
}

std::string EscapeJsonString(const std::wstring& value) {
    std::ostringstream output;
    for (unsigned char ch : WideToUtf8(value)) {
        switch (ch) {
            case '"':
                output << "\\\"";
                break;
            case '\\':
                output << "\\\\";
                break;
            case '\b':
                output << "\\b";
                break;
            case '\f':
                output << "\\f";
                break;
            case '\n':
                output << "\\n";
                break;
            case '\r':
                output << "\\r";
                break;
            case '\t':
                output << "\\t";
                break;
            default:
                if (ch < 0x20) {
                    output << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(ch)
                           << std::dec << std::setfill(' ');
                } else {
                    output << static_cast<char>(ch);
                }
                break;
        }
    }
    return output.str();
}

std::optional<std::size_t> FindFieldValue(std::string_view text, std::string_view key) {
    const std::string needle = "\"" + std::string(key) + "\"";
    const std::size_t key_position = text.find(needle);
    if (key_position == std::string_view::npos) {
        return std::nullopt;
    }

    std::size_t position = key_position + needle.size();
    while (position < text.size() && std::isspace(static_cast<unsigned char>(text[position]))) {
        ++position;
    }
    if (position >= text.size() || text[position] != ':') {
        return std::nullopt;
    }
    ++position;
    while (position < text.size() && std::isspace(static_cast<unsigned char>(text[position]))) {
        ++position;
    }
    if (position >= text.size()) {
        return std::nullopt;
    }
    return position;
}

std::optional<bool> BoolField(std::string_view text, std::string_view key) {
    const auto position = FindFieldValue(text, key);
    if (!position) {
        return std::nullopt;
    }

    if (text.substr(*position, 4) == "true") {
        return true;
    }
    if (text.substr(*position, 5) == "false") {
        return false;
    }
    return std::nullopt;
}

std::optional<int> IntField(std::string_view text, std::string_view key) {
    const auto position = FindFieldValue(text, key);
    if (!position) {
        return std::nullopt;
    }

    std::size_t index = *position;
    bool negative = false;
    if (text[index] == '-') {
        negative = true;
        ++index;
    }
    if (index >= text.size() || !std::isdigit(static_cast<unsigned char>(text[index]))) {
        return std::nullopt;
    }

    int value = 0;
    while (index < text.size() && std::isdigit(static_cast<unsigned char>(text[index]))) {
        const int digit = text[index] - '0';
        if (value > (INT_MAX - digit) / 10) {
            return std::nullopt;
        }
        value = value * 10 + digit;
        ++index;
    }

    return negative ? -value : value;
}

std::optional<std::string> StringField(std::string_view text, std::string_view key) {
    const auto start = FindFieldValue(text, key);
    if (!start || text[*start] != '"') {
        return std::nullopt;
    }

    std::string output;
    for (std::size_t index = *start + 1; index < text.size(); ++index) {
        const char current = text[index];
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
        if (++index >= text.size()) {
            return std::nullopt;
        }
        switch (text[index]) {
            case '"':
            case '\\':
            case '/':
                output.push_back(text[index]);
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
            default:
                return std::nullopt;
        }
    }

    return std::nullopt;
}

bool IsValidSettings(const UserSettings& settings) {
    return settings.flicker_intensity >= 0 && settings.flicker_intensity <= 2 &&
           !settings.deck_layout_mode.empty() && !settings.homepage.empty() &&
           !settings.search_engine_url.empty();
}

}  // namespace

std::filesystem::path SettingsStore::DefaultSettingsPath() {
    return common::AppDataDirectory() / "settings.json";
}

bool SettingsStore::Initialize(std::filesystem::path settings_path, common::Logger& logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_ = &logger;
    settings_path_ = std::move(settings_path);
    settings_ = UserSettings{};

    if (!LoadLocked()) {
        if (!RenameCorruptedFileLocked()) {
            logger_->Error("Unable to recover corrupted settings file.");
            return false;
        }
        settings_ = UserSettings{};
        if (!WriteLocked()) {
            logger_->Error("Unable to write default settings after corruption recovery.");
            return false;
        }
    }

    logger_->Info("Settings store ready: " + NarrowForLog(settings_path_));
    return true;
}

UserSettings SettingsStore::Settings() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return settings_;
}

bool SettingsStore::Save(UserSettings settings) {
    if (!IsValidSettings(settings)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = std::move(settings);
    return WriteLocked();
}

std::filesystem::path SettingsStore::path() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return settings_path_;
}

bool SettingsStore::LoadLocked() {
    auto fail = [this](std::string_view reason) {
        if (logger_ != nullptr) {
            logger_->Error(std::string("Settings file invalid: ") + std::string(reason));
        }
        return false;
    };

    if (settings_path_.empty()) {
        return fail("empty path");
    }

    std::error_code error;
    if (!std::filesystem::exists(settings_path_, error)) {
        return WriteLocked();
    }

    std::ifstream input(settings_path_, std::ios::binary);
    if (!input.is_open()) {
        return fail("could not open file");
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    const std::string json_text = buffer.str();
    const auto first = json_text.find_first_not_of(" \t\r\n");
    const auto last = json_text.find_last_not_of(" \t\r\n");
    if (first == std::string::npos || json_text[first] != '{' || json_text[last] != '}') {
        return fail("root is not an object");
    }

    UserSettings loaded;
    const auto terminal = BoolField(json_text, "terminalModeEnabled");
    const auto scanlines = BoolField(json_text, "scanlinesEnabled");
    const auto glow = BoolField(json_text, "glowEnabled");
    const auto flicker = IntField(json_text, "flickerIntensity");
    const auto keep_deck_open = BoolField(json_text, "keepDeckOpenAfterNodeOpen");
    const auto deck_layout = StringField(json_text, "deckLayoutMode");
    const auto homepage = StringField(json_text, "homepage");
    const auto search_engine = StringField(json_text, "searchEngineUrl");
    if (!terminal || !scanlines || !glow || !flicker || !deck_layout || !homepage || !search_engine) {
        return fail("required field missing or invalid");
    }

    loaded.terminal_mode_enabled = *terminal;
    loaded.scanlines_enabled = *scanlines;
    loaded.glow_enabled = *glow;
    loaded.flicker_intensity = *flicker;
    loaded.keep_deck_open_after_node_open = keep_deck_open.value_or(false);
    loaded.deck_layout_mode = Utf8ToWide(*deck_layout);
    loaded.homepage = Utf8ToWide(*homepage);
    loaded.search_engine_url = Utf8ToWide(*search_engine);
    if (!IsValidSettings(loaded)) {
        return fail("settings values are invalid");
    }

    settings_ = std::move(loaded);
    return true;
}

bool SettingsStore::WriteLocked() {
    if (!IsValidSettings(settings_)) {
        return false;
    }

    std::error_code error;
    std::filesystem::create_directories(settings_path_.parent_path(), error);
    if (error) {
        return false;
    }

    const std::filesystem::path temp_path = settings_path_.string() + ".tmp";
    {
        std::ofstream output(temp_path, std::ios::binary | std::ios::trunc);
        if (!output.is_open()) {
            return false;
        }

        output << "{\n";
        output << "  \"version\": 1,\n";
        output << "  \"terminalModeEnabled\": " << (settings_.terminal_mode_enabled ? "true" : "false") << ",\n";
        output << "  \"scanlinesEnabled\": " << (settings_.scanlines_enabled ? "true" : "false") << ",\n";
        output << "  \"glowEnabled\": " << (settings_.glow_enabled ? "true" : "false") << ",\n";
        output << "  \"flickerIntensity\": " << settings_.flicker_intensity << ",\n";
        output << "  \"keepDeckOpenAfterNodeOpen\": " << (settings_.keep_deck_open_after_node_open ? "true" : "false") << ",\n";
        output << "  \"deckLayoutMode\": \"" << EscapeJsonString(settings_.deck_layout_mode) << "\",\n";
        output << "  \"homepage\": \"" << EscapeJsonString(settings_.homepage) << "\",\n";
        output << "  \"searchEngineUrl\": \"" << EscapeJsonString(settings_.search_engine_url) << "\"\n";
        output << "}\n";
        output.flush();
        if (!output.good()) {
            return false;
        }
    }

    if (!common::ReplaceFile(temp_path, settings_path_)) {
        std::filesystem::remove(temp_path, error);
        return false;
    }

    return true;
}

bool SettingsStore::RenameCorruptedFileLocked() {
    std::error_code error;
    if (!std::filesystem::exists(settings_path_, error)) {
        return true;
    }

    const std::filesystem::path corrupted_path =
        settings_path_.parent_path() /
        (settings_path_.filename().wstring() + L".corrupt." + Utf8ToWide(CurrentFileTimestamp()) + L".bak");

    if (!common::ReplaceFile(settings_path_, corrupted_path)) {
        return false;
    }

    if (logger_ != nullptr) {
        logger_->Error("Corrupted settings file renamed to: " + NarrowForLog(corrupted_path));
    }
    return true;
}

}  // namespace cyberdeck::settings
