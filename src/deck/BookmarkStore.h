#pragma once

#include "common/Logger.h"
#include "deck/BookmarkNode.h"

#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cyberdeck::deck {

class BookmarkStore {
public:
    BookmarkStore() = default;
    BookmarkStore(const BookmarkStore&) = delete;
    BookmarkStore& operator=(const BookmarkStore&) = delete;

    static std::filesystem::path DefaultBookmarksPath();

    bool Initialize(std::filesystem::path bookmarks_path, common::Logger& logger);
    std::vector<BookmarkNode> LoadBookmarks() const;
    bool SaveBookmarks(std::vector<BookmarkNode> nodes);
    bool AddBookmark(BookmarkNode node);
    bool UpdateBookmark(BookmarkNode node);
    bool DeleteBookmark(std::wstring_view id);
    std::optional<BookmarkNode> FindBookmarkById(std::wstring_view id) const;
    std::filesystem::path path() const;

private:
    bool LoadLocked();
    bool WriteLocked();
    bool RenameCorruptedFileLocked();

    mutable std::mutex mutex_;
    std::filesystem::path bookmarks_path_;
    common::Logger* logger_ = nullptr;
    std::vector<BookmarkNode> nodes_;
};

}  // namespace cyberdeck::deck
