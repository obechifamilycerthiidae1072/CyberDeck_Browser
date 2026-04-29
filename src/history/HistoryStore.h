#pragma once

#include "common/Logger.h"

#include <filesystem>
#include <mutex>
#include <string>
#include <vector>

namespace cyberdeck::history {

struct HistoryEntry {
    int id = 0;
    std::wstring title;
    std::wstring url;
    int visit_count = 0;
    std::string first_visited_utc;
    std::string last_visited_utc;
};

class HistoryStore {
public:
    HistoryStore() = default;
    HistoryStore(const HistoryStore&) = delete;
    HistoryStore& operator=(const HistoryStore&) = delete;

    static std::filesystem::path DefaultHistoryPath();

    bool Initialize(std::filesystem::path history_path, common::Logger& logger);
    bool RecordVisit(const std::wstring& title, const std::wstring& url);
    std::vector<HistoryEntry> Entries() const;
    std::filesystem::path path() const;

private:
    bool LoadLocked();
    bool WriteLocked();
    bool RenameCorruptedFileLocked();

    mutable std::mutex mutex_;
    std::filesystem::path history_path_;
    common::Logger* logger_ = nullptr;
    std::vector<HistoryEntry> entries_;
    int next_id_ = 1;
};

}  // namespace cyberdeck::history
