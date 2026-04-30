#include "common/Platform.h"

#include <unistd.h>

#include <cstdlib>
#include <system_error>

namespace cyberdeck::common {
namespace {

std::filesystem::path EnvironmentPath(const char* name) {
    const char* value = std::getenv(name);
    return value == nullptr || *value == '\0' ? std::filesystem::path{} : std::filesystem::path(value);
}

std::filesystem::path EnvironmentAbsolutePath(const char* name) {
    const std::filesystem::path path = EnvironmentPath(name);
    return path.is_absolute() ? path : std::filesystem::path{};
}

void AppendUtf8(std::string& output, char32_t codepoint) {
    if (codepoint <= 0x7F) {
        output.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        output.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        output.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x10FFFF) {
        output.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
}

bool ReadContinuation(std::string_view value, std::size_t& index, char32_t& codepoint) {
    if (index >= value.size()) {
        return false;
    }
    const unsigned char next = static_cast<unsigned char>(value[index]);
    if ((next & 0xC0) != 0x80) {
        return false;
    }
    codepoint = (codepoint << 6) | static_cast<char32_t>(next & 0x3F);
    ++index;
    return true;
}

}  // namespace

std::filesystem::path AppDataDirectory() {
    std::filesystem::path root = EnvironmentAbsolutePath("XDG_DATA_HOME");
    if (root.empty()) {
        const std::filesystem::path home = EnvironmentAbsolutePath("HOME");
        if (!home.empty()) {
            root = home / ".local" / "share";
        }
    }

    if (root.empty()) {
        root = std::filesystem::current_path() / "dev" / "appdata";
    }

    return root / "cyberdeck-browser";
}

bool ReplaceFile(const std::filesystem::path& source, const std::filesystem::path& destination) {
    std::error_code error;
    std::filesystem::rename(source, destination, error);
    return !error;
}

std::uint32_t CurrentProcessId() {
    return static_cast<std::uint32_t>(getpid());
}

std::string WideToUtf8(std::wstring_view value) {
    std::string output;
    for (wchar_t ch : value) {
        AppendUtf8(output, static_cast<char32_t>(ch));
    }
    return output;
}

std::wstring Utf8ToWide(std::string_view value) {
    std::wstring output;
    for (std::size_t index = 0; index < value.size();) {
        const unsigned char first = static_cast<unsigned char>(value[index++]);
        char32_t codepoint = 0;
        int continuation_count = 0;

        if ((first & 0x80) == 0) {
            codepoint = first;
        } else if ((first & 0xE0) == 0xC0) {
            codepoint = first & 0x1F;
            continuation_count = 1;
        } else if ((first & 0xF0) == 0xE0) {
            codepoint = first & 0x0F;
            continuation_count = 2;
        } else if ((first & 0xF8) == 0xF0) {
            codepoint = first & 0x07;
            continuation_count = 3;
        } else {
            return {};
        }

        for (int count = 0; count < continuation_count; ++count) {
            if (!ReadContinuation(value, index, codepoint)) {
                return {};
            }
        }

        const char32_t minimum =
            continuation_count == 0 ? 0 :
            continuation_count == 1 ? 0x80 :
            continuation_count == 2 ? 0x800 :
                                      0x10000;
        if (codepoint < minimum || codepoint > 0x10FFFF ||
            (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
            return {};
        }
        output.push_back(static_cast<wchar_t>(codepoint));
    }
    return output;
}

}  // namespace cyberdeck::common
