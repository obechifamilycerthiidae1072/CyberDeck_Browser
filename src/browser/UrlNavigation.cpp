#include "browser/UrlNavigation.h"

#include <algorithm>
#include <cwctype>
#include <sstream>
#include <utility>

namespace cyberdeck::browser {
namespace {

constexpr std::wstring_view kDefaultSearchUrlPrefix = L"https://duckduckgo.com/?q=";

std::wstring Trim(std::wstring_view input) {
    auto begin = input.begin();
    auto end = input.end();

    while (begin != end && std::iswspace(*begin)) {
        ++begin;
    }
    while (begin != end && std::iswspace(*(end - 1))) {
        --end;
    }

    return std::wstring(begin, end);
}

std::wstring ToLower(std::wstring_view input) {
    std::wstring lowered(input);
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](wchar_t value) {
        return static_cast<wchar_t>(std::towlower(value));
    });
    return lowered;
}

bool ContainsWhitespace(std::wstring_view input) {
    return std::any_of(input.begin(), input.end(), [](wchar_t value) {
        return std::iswspace(value) != 0;
    });
}

bool StartsWith(std::wstring_view value, std::wstring_view prefix) {
    return value.size() >= prefix.size() && value.substr(0, prefix.size()) == prefix;
}

bool IsWindowsDrivePath(std::wstring_view input) {
    return input.size() >= 3 && std::iswalpha(input[0]) && input[1] == L':' &&
           (input[2] == L'\\' || input[2] == L'/');
}

bool LooksLikeLocalPath(std::wstring_view input) {
    return IsWindowsDrivePath(input) || StartsWith(input, L"\\\\") || StartsWith(input, L"./") ||
           StartsWith(input, L".\\") || StartsWith(input, L"../") || StartsWith(input, L"..\\") ||
           StartsWith(input, L"/");
}

std::wstring ExtractScheme(std::wstring_view input) {
    const auto colon = input.find(L':');
    if (colon == std::wstring_view::npos) {
        return {};
    }

    const auto after_colon = colon + 1;
    const auto port_end = input.find_first_of(L"/?#", after_colon);
    const auto port = input.substr(after_colon, port_end == std::wstring_view::npos ? port_end : port_end - after_colon);
    if (!port.empty() &&
        std::all_of(port.begin(), port.end(), [](wchar_t value) { return std::iswdigit(value) != 0; })) {
        return {};
    }

    const auto slash = input.find_first_of(L"/?#");
    if (slash != std::wstring_view::npos && slash < colon) {
        return {};
    }

    if (colon == 1 && std::iswalpha(input[0])) {
        return {};
    }

    for (std::size_t index = 0; index < colon; ++index) {
        const wchar_t value = input[index];
        const bool valid = std::iswalnum(value) || value == L'+' || value == L'-' || value == L'.';
        if (!valid) {
            return {};
        }
    }

    return ToLower(input.substr(0, colon));
}

bool LooksLikeUrlWithoutScheme(std::wstring_view input) {
    if (input.empty() || ContainsWhitespace(input)) {
        return false;
    }

    if (input == L"localhost" || StartsWith(input, L"localhost:")) {
        return true;
    }

    return input.find(L'.') != std::wstring_view::npos;
}

bool IsUnreservedQueryChar(wchar_t value) {
    return (value >= L'a' && value <= L'z') || (value >= L'A' && value <= L'Z') ||
           (value >= L'0' && value <= L'9') || value == L'-' || value == L'_' ||
           value == L'.' || value == L'~';
}

std::wstring PercentEncodeUtf8(std::wstring_view input) {
    std::wostringstream encoded;
    encoded << std::uppercase << std::hex;

    for (wchar_t value : input) {
        if (value == L' ') {
            encoded << L'+';
        } else if (IsUnreservedQueryChar(value)) {
            encoded << value;
        } else if (value <= 0x7F) {
            encoded << L'%' << static_cast<int>((value >> 4) & 0xF)
                    << static_cast<int>(value & 0xF);
        } else {
            encoded << L'+';
        }
    }

    return encoded.str();
}

NormalizedNavigation Block(std::wstring reason) {
    return {.decision = NavigationDecision::kBlocked, .target_url = {}, .reason = std::move(reason)};
}

NormalizedNavigation Navigate(std::wstring target) {
    return {.decision = NavigationDecision::kNavigate, .target_url = std::move(target), .reason = {}};
}

}  // namespace

ProtocolDecision ClassifyNavigationProtocol(std::wstring_view url) {
    const std::wstring trimmed = Trim(url);
    const std::wstring scheme = ExtractScheme(trimmed);
    if (scheme.empty() || scheme == L"http" || scheme == L"https" || scheme == L"about") {
        return {.action = ProtocolAction::kAllow, .scheme = scheme, .reason = {}};
    }

    if (scheme == L"mailto" || scheme == L"tel") {
        return {
            .action = ProtocolAction::kConfirmExternal,
            .scheme = scheme,
            .reason = L"This link wants to open an external application."};
    }

    if (scheme == L"file") {
        return {
            .action = ProtocolAction::kBlock,
            .scheme = scheme,
            .reason = L"Local file browsing is blocked by default."};
    }

    if (scheme == L"javascript" || scheme == L"data") {
        return {
            .action = ProtocolAction::kBlock,
            .scheme = scheme,
            .reason = L"Script and data URL schemes are blocked outside normal page execution."};
    }

    return {
        .action = ProtocolAction::kConfirmExternal,
        .scheme = scheme,
        .reason = L"This custom protocol may launch another application."};
}

NormalizedNavigation NormalizeAddressBarInput(std::wstring_view input) {
    std::wstring trimmed = Trim(input);
    if (trimmed.empty()) {
        return {.decision = NavigationDecision::kEmpty, .target_url = {}, .reason = L"Address bar input was empty."};
    }

    if (LooksLikeLocalPath(trimmed)) {
        return Block(L"Local file paths are blocked from the address bar by default.");
    }

    const std::wstring scheme = ExtractScheme(trimmed);
    if (!scheme.empty()) {
        if (scheme == L"http" || scheme == L"https") {
            return Navigate(trimmed);
        }

        const ProtocolDecision protocol = ClassifyNavigationProtocol(trimmed);
        if (protocol.action == ProtocolAction::kConfirmExternal) {
            return Block(L"External application protocols must be opened from page links after confirmation: " + scheme);
        }
        return Block(protocol.reason.empty() ? L"Blocked unsupported or dangerous URL scheme: " + scheme : protocol.reason);
    }

    if (LooksLikeUrlWithoutScheme(trimmed)) {
        return Navigate(L"https://" + trimmed);
    }

    return Navigate(std::wstring(kDefaultSearchUrlPrefix) + PercentEncodeUtf8(trimmed));
}

}  // namespace cyberdeck::browser
