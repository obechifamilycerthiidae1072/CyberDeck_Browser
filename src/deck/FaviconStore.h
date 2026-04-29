#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace cyberdeck::deck {

class FaviconStore {
public:
    static std::filesystem::path DefaultFaviconsDirectory();
    static std::optional<std::wstring> EnsurePlaceholderFavicon(
        const std::filesystem::path& favicons_directory,
        std::wstring_view url,
        std::wstring_view title);
};

}  // namespace cyberdeck::deck
