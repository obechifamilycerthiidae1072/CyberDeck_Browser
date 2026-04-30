#include "history/HistoryStore.h"

#include "common/Logger.h"
#include "common/Platform.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

std::filesystem::path TestRoot() {
    return std::filesystem::temp_directory_path() /
           ("CyberDeckHistoryStoreTests-" + std::to_string(cyberdeck::common::CurrentProcessId()));
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

    const std::filesystem::path history_path = root / "CyberDeckBrowser" / "history.json";

    bool passed = true;
    {
        cyberdeck::history::HistoryStore store;
        passed = Expect(store.Initialize(history_path, logger), "Initialize should create a history file.") && passed;
        passed = Expect(store.RecordVisit(L"Example Domain", L"https://www.example.com/"), "RecordVisit should write first visit.") && passed;
        passed = Expect(store.RecordVisit(L"Example Domain", L"https://www.example.com/"), "RecordVisit should update repeat visit.") && passed;

        const auto entries = store.Entries();
        passed = Expect(entries.size() == 1, "Repeat visits should update one entry.") && passed;
        if (!entries.empty()) {
            passed = Expect(entries[0].id == 1, "First history id should be 1.") && passed;
            passed = Expect(entries[0].visit_count == 2, "Repeat visit count should be 2.") && passed;
            passed = Expect(!entries[0].first_visited_utc.empty(), "First visited timestamp should be set.") && passed;
            passed = Expect(!entries[0].last_visited_utc.empty(), "Last visited timestamp should be set.") && passed;
        }
    }

    {
        std::ifstream saved_before_reload(history_path, std::ios::binary);
        std::ostringstream saved_before_reload_text;
        saved_before_reload_text << saved_before_reload.rdbuf();

        cyberdeck::history::HistoryStore store;
        passed = Expect(store.Initialize(history_path, logger), "Initialize should reload saved history.") && passed;
        const auto entries = store.Entries();
        if (!Expect(entries.size() == 1, "Saved history should survive restart.")) {
            std::cerr << "Saved history JSON before failed reload:\n" << saved_before_reload_text.str() << '\n';
            passed = false;
        }
        if (!entries.empty()) {
            passed = Expect(entries[0].visit_count == 2, "Saved visit count should survive restart.") && passed;
        }
    }

    passed = Expect(WriteText(history_path, "{ not valid json"), "Test should write corrupted history.") && passed;

    {
        cyberdeck::history::HistoryStore store;
        passed = Expect(store.Initialize(history_path, logger), "Corrupted history should be recovered.") && passed;
        passed = Expect(store.Entries().empty(), "Recovered history should start clean.") && passed;

        int corrupt_backups = 0;
        for (const auto& item : std::filesystem::directory_iterator(history_path.parent_path())) {
            const std::wstring name = item.path().filename().wstring();
            if (name.find(L"history.json.corrupt.") == 0) {
                ++corrupt_backups;
            }
        }
        passed = Expect(corrupt_backups >= 1, "Corrupted file should be renamed with a timestamp.") && passed;
    }

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
