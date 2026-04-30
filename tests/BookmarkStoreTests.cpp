#include "deck/BookmarkStore.h"

#include "common/Logger.h"
#include "common/Platform.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

namespace {

std::filesystem::path TestRoot() {
    return std::filesystem::temp_directory_path() /
           ("CyberDeckBookmarkStoreTests-" + std::to_string(cyberdeck::common::CurrentProcessId()));
}

bool Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

bool WriteText(const std::filesystem::path& path, const std::string& text) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output << text;
    return output.good();
}

bool HasBookmark(
    const std::vector<cyberdeck::deck::BookmarkNode>& nodes,
    std::wstring_view id,
    std::wstring_view url) {
    for (const cyberdeck::deck::BookmarkNode& node : nodes) {
        if (node.id == id && node.url == url) {
            return true;
        }
    }
    return false;
}

cyberdeck::deck::BookmarkNode TestNode(std::wstring id, std::wstring title, std::wstring url) {
    cyberdeck::deck::BookmarkNode node;
    node.id = std::move(id);
    node.title = std::move(title);
    node.url = std::move(url);
    node.shape_type = cyberdeck::deck::BookmarkNodeShapeType::Hex;
    node.color_theme = cyberdeck::deck::BookmarkNodeColorTheme::Green;
    node.created_utc = "2026-04-29T04:00:00Z";
    node.updated_utc = "2026-04-29T04:00:00Z";
    node.visit_count = 0;
    return node;
}

}  // namespace

int main() {
    const std::filesystem::path root = TestRoot();
    std::error_code error;
    std::filesystem::remove_all(root, error);

    cyberdeck::common::Logger logger;
    if (!logger.Initialize(root / "test.log")) {
        std::cerr << "Failed to initialize test logger.\n";
        return EXIT_FAILURE;
    }

    const std::filesystem::path bookmarks_path = root / "CyberDeckBrowser" / "bookmarks.json";
    bool passed = true;

    {
        cyberdeck::deck::BookmarkStore store;
        passed = Expect(store.Initialize(bookmarks_path, logger), "Initialize should create a bookmark file.") && passed;
        const auto default_nodes = store.LoadBookmarks();
        passed = Expect(default_nodes.size() == 4, "New bookmark file should start with default bookmarks.") && passed;
        passed = Expect(
                     HasBookmark(default_nodes, L"default-google", L"https://www.google.com"),
                     "Default bookmarks should include Google.") &&
                 passed;
        passed = Expect(
                     HasBookmark(default_nodes, L"default-reddit", L"https://www.reddit.com"),
                     "Default bookmarks should include Reddit.") &&
                 passed;
        passed = Expect(
                     HasBookmark(default_nodes, L"default-github", L"https://github.com"),
                     "Default bookmarks should include GitHub.") &&
                 passed;
        passed = Expect(
                     HasBookmark(default_nodes, L"default-chatgpt", L"https://chatgpt.com"),
                     "Default bookmarks should include ChatGPT.") &&
                 passed;

        auto node = TestNode(L"node-example", L"Example Domain", L"https://example.com");
        passed = Expect(store.AddBookmark(node), "AddBookmark should persist a valid node.") && passed;
        passed = Expect(!store.AddBookmark(node), "AddBookmark should reject duplicate ids.") && passed;

        auto found = store.FindBookmarkById(L"node-example");
        passed = Expect(found.has_value(), "FindBookmarkById should find the saved node.") && passed;
        if (found) {
            passed = Expect(found->title == L"Example Domain", "Found node should match saved title.") && passed;
        }

        node.title = L"Example Updated";
        node.url = L"https://example.org";
        node.shape_type = cyberdeck::deck::BookmarkNodeShapeType::Cube;
        node.color_theme = cyberdeck::deck::BookmarkNodeColorTheme::Yellow;
        node.updated_utc = "2026-04-29T04:10:00Z";
        node.tags = {L"daily", L"docs"};
        passed = Expect(store.UpdateBookmark(node), "UpdateBookmark should persist changes.") && passed;
    }

    {
        cyberdeck::deck::BookmarkStore store;
        passed = Expect(store.Initialize(bookmarks_path, logger), "Initialize should reload saved bookmarks.") && passed;
        const auto nodes = store.LoadBookmarks();
        passed = Expect(nodes.size() == 5, "Saved bookmarks and defaults should survive restart.") && passed;
        const auto updated_node = store.FindBookmarkById(L"node-example");
        passed = Expect(updated_node.has_value(), "Updated bookmark should survive restart.") && passed;
        if (updated_node) {
            passed = Expect(updated_node->title == L"Example Updated", "Updated title should survive restart.") && passed;
            passed = Expect(updated_node->url == L"https://example.org", "Updated URL should survive restart.") && passed;
            passed = Expect(
                         updated_node->shape_type == cyberdeck::deck::BookmarkNodeShapeType::Cube,
                         "Updated shape should survive restart.") &&
                     passed;
            passed = Expect(
                         updated_node->color_theme == cyberdeck::deck::BookmarkNodeColorTheme::Yellow,
                         "Updated color should survive restart.") &&
                     passed;
            passed = Expect(updated_node->tags.size() == 2, "Updated tags should survive restart.") && passed;
        }

        auto opened = updated_node.value_or(cyberdeck::deck::BookmarkNode{});
        opened.last_visited_utc = "2026-04-29T04:20:00Z";
        opened.updated_utc = "2026-04-29T04:20:00Z";
        opened.visit_count += 1;
        passed = Expect(store.UpdateBookmark(opened), "UpdateBookmark should persist visit metadata.") && passed;

        const auto visited = store.FindBookmarkById(L"node-example");
        passed = Expect(visited.has_value(), "Visited bookmark should still be findable.") && passed;
        if (visited) {
            passed = Expect(visited->last_visited_utc.has_value(), "Visited bookmark should store lastVisitedUtc.") &&
                     passed;
            passed = Expect(visited->visit_count == 1, "Visited bookmark should increment visit count.") && passed;
        }

        passed = Expect(store.DeleteBookmark(L"node-example"), "DeleteBookmark should persist removal.") && passed;
        passed = Expect(store.LoadBookmarks().size() == 4, "Default bookmarks should remain after deleting one custom node.") &&
                 passed;
        passed = Expect(store.SaveBookmarks({}), "SaveBookmarks should allow the user to remove all bookmarks.") && passed;
    }

    {
        cyberdeck::deck::BookmarkStore store;
        passed = Expect(store.Initialize(bookmarks_path, logger), "Initialize should reload after delete.") && passed;
        passed = Expect(
                     store.LoadBookmarks().empty(),
                     "Default bookmarks should not reappear after the user saves an empty list.") &&
                 passed;
    }

    {
        const std::filesystem::path legacy_empty_path = root / "CyberDeckBrowser" / "legacy-empty-bookmarks.json";
        passed = Expect(
                     WriteText(legacy_empty_path, "{\n  \"version\": 1,\n  \"nodes\": []\n}\n"),
                     "Test should write legacy empty bookmark storage.") &&
                 passed;

        cyberdeck::deck::BookmarkStore store;
        passed = Expect(store.Initialize(legacy_empty_path, logger), "Legacy empty storage should initialize.") && passed;
        passed = Expect(
                     store.LoadBookmarks().size() == 4,
                     "Legacy empty storage should receive default bookmarks once.") &&
                 passed;
        passed = Expect(store.SaveBookmarks({}), "Legacy defaults should be removable by saving an empty list.") && passed;

        cyberdeck::deck::BookmarkStore reloaded;
        passed = Expect(reloaded.Initialize(legacy_empty_path, logger), "Legacy path should reload after empty save.") &&
                 passed;
        passed = Expect(
                     reloaded.LoadBookmarks().empty(),
                     "Legacy default bookmarks should not reappear after empty save.") &&
                 passed;
    }

    {
        cyberdeck::deck::BookmarkStore store;
        auto unsafe = TestNode(L"node-unsafe", L"Unsafe", L"file:///C:/Windows/win.ini");
        passed = Expect(store.Initialize(bookmarks_path, logger), "Initialize before invalid write should work.") && passed;
        passed = Expect(!store.AddBookmark(unsafe), "AddBookmark should reject invalid nodes.") && passed;
    }

    passed = Expect(WriteText(bookmarks_path, "{ not valid json"), "Test should write corrupted bookmarks.") && passed;

    {
        cyberdeck::deck::BookmarkStore store;
        passed = Expect(store.Initialize(bookmarks_path, logger), "Corrupted bookmarks should be recovered.") && passed;
        passed = Expect(store.LoadBookmarks().empty(), "Recovered bookmarks should start clean.") && passed;

        int corrupt_backups = 0;
        for (const auto& item : std::filesystem::directory_iterator(bookmarks_path.parent_path())) {
            const std::wstring name = item.path().filename().wstring();
            if (name.find(L"bookmarks.json.corrupt.") == 0) {
                ++corrupt_backups;
            }
        }
        passed = Expect(corrupt_backups >= 1, "Corrupted file should be renamed with a timestamp.") && passed;
    }

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
