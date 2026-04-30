#include "settings/SettingsStore.h"

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
           ("CyberDeckSettingsStoreTests-" + std::to_string(cyberdeck::common::CurrentProcessId()));
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

    const std::filesystem::path settings_path = root / "CyberDeckBrowser" / "settings.json";
    bool passed = true;

    {
        cyberdeck::settings::SettingsStore store;
        passed = Expect(store.Initialize(settings_path, logger), "Initialize should create a settings file.") && passed;
        const auto defaults = store.Settings();
        passed = Expect(!defaults.terminal_mode_enabled, "Terminal Mode should default off.") && passed;
        passed = Expect(defaults.scanlines_enabled, "Scanlines should default on.") && passed;
        passed = Expect(defaults.glow_enabled, "Glow should default on.") && passed;
        passed = Expect(defaults.flicker_intensity == 0, "Flicker should default off.") && passed;
        passed = Expect(!defaults.keep_deck_open_after_node_open, "Deck should default to exiting after opening a Node.") &&
                 passed;
        passed = Expect(defaults.deck_layout_mode == L"hex-ring", "Deck layout should default to Hex Ring.") && passed;

        auto changed = defaults;
        changed.terminal_mode_enabled = true;
        changed.scanlines_enabled = false;
        changed.glow_enabled = false;
        changed.flicker_intensity = 2;
        changed.keep_deck_open_after_node_open = true;
        changed.deck_layout_mode = L"grid";
        changed.homepage = L"https://example.test";
        changed.search_engine_url = L"https://search.example/?q={query}";
        passed = Expect(store.Save(changed), "Save should persist changed settings.") && passed;
    }

    {
        cyberdeck::settings::SettingsStore store;
        passed = Expect(store.Initialize(settings_path, logger), "Initialize should reload saved settings.") && passed;
        const auto loaded = store.Settings();
        passed = Expect(loaded.terminal_mode_enabled, "Terminal Mode should survive restart.") && passed;
        passed = Expect(!loaded.scanlines_enabled, "Scanline setting should survive restart.") && passed;
        passed = Expect(!loaded.glow_enabled, "Glow setting should survive restart.") && passed;
        passed = Expect(loaded.flicker_intensity == 2, "Flicker intensity should survive restart.") && passed;
        passed = Expect(loaded.keep_deck_open_after_node_open, "Node open behavior should survive restart.") && passed;
        passed = Expect(loaded.deck_layout_mode == L"grid", "Deck layout should survive restart.") && passed;
        passed = Expect(loaded.homepage == L"https://example.test", "Homepage should survive restart.") && passed;
    }

    passed = Expect(
                 WriteText(
                     settings_path,
                     "{\n"
                     "  \"version\": 1,\n"
                     "  \"terminalModeEnabled\": false,\n"
                     "  \"scanlinesEnabled\": true,\n"
                     "  \"glowEnabled\": true,\n"
                     "  \"flickerIntensity\": 0,\n"
                     "  \"deckLayoutMode\": \"orbit\",\n"
                     "  \"homepage\": \"https://example.com\",\n"
                     "  \"searchEngineUrl\": \"https://duckduckgo.com/?q={query}\"\n"
                     "}\n"),
                 "Test should write legacy settings without Node open behavior.") &&
             passed;

    {
        cyberdeck::settings::SettingsStore store;
        passed = Expect(store.Initialize(settings_path, logger), "Legacy settings should load with new defaults.") &&
                 passed;
        const auto loaded = store.Settings();
        passed = Expect(
                     !loaded.keep_deck_open_after_node_open,
                     "Legacy settings should default to exiting Deck after opening a Node.") &&
                 passed;
    }

    passed = Expect(WriteText(settings_path, "{ not valid json"), "Test should write corrupted settings.") && passed;

    {
        cyberdeck::settings::SettingsStore store;
        passed = Expect(store.Initialize(settings_path, logger), "Corrupted settings should recover.") && passed;
        const auto recovered = store.Settings();
        passed = Expect(!recovered.terminal_mode_enabled, "Recovered settings should use defaults.") && passed;

        int corrupt_backups = 0;
        for (const auto& item : std::filesystem::directory_iterator(settings_path.parent_path())) {
            const std::wstring name = item.path().filename().wstring();
            if (name.find(L"settings.json.corrupt.") == 0) {
                ++corrupt_backups;
            }
        }
        passed = Expect(corrupt_backups >= 1, "Corrupted file should be renamed with a timestamp.") && passed;
    }

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
