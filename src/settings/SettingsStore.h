#pragma once

#include "common/Logger.h"

#include <filesystem>
#include <mutex>
#include <string>

namespace cyberdeck::settings {

struct UserSettings {
    bool terminal_mode_enabled = false;
    bool scanlines_enabled = true;
    bool glow_enabled = true;
    int flicker_intensity = 0;
    bool keep_deck_open_after_node_open = false;
    std::wstring deck_layout_mode = L"hex-ring";
    std::wstring homepage = L"https://www.example.com";
    std::wstring search_engine_url = L"https://duckduckgo.com/?q={query}";
};

class SettingsStore {
public:
    SettingsStore() = default;
    SettingsStore(const SettingsStore&) = delete;
    SettingsStore& operator=(const SettingsStore&) = delete;

    static std::filesystem::path DefaultSettingsPath();

    bool Initialize(std::filesystem::path settings_path, common::Logger& logger);
    UserSettings Settings() const;
    bool Save(UserSettings settings);
    std::filesystem::path path() const;

private:
    bool LoadLocked();
    bool WriteLocked();
    bool RenameCorruptedFileLocked();

    mutable std::mutex mutex_;
    std::filesystem::path settings_path_;
    common::Logger* logger_ = nullptr;
    UserSettings settings_;
};

}  // namespace cyberdeck::settings
