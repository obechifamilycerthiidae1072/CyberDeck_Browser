#include "browser/UrlNavigation.h"
#include "common/AppInfo.h"
#include "common/Logger.h"
#include "common/Platform.h"
#include "deck/BookmarkStore.h"
#include "history/HistoryStore.h"
#include "settings/SettingsStore.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

std::string Narrow(std::wstring_view value) {
    return cyberdeck::common::WideToUtf8(value);
}

const char* DecisionText(cyberdeck::browser::NavigationDecision decision) {
    switch (decision) {
        case cyberdeck::browser::NavigationDecision::kNavigate:
            return "navigate";
        case cyberdeck::browser::NavigationDecision::kBlocked:
            return "blocked";
        case cyberdeck::browser::NavigationDecision::kEmpty:
            return "empty";
    }
    return "unknown";
}

}  // namespace

int main(int argc, char** argv) {
    cyberdeck::common::Logger logger;
    const bool logging_ready = logger.Initialize(cyberdeck::common::Logger::DefaultLogPath());
    if (logging_ready) {
        logger.Info("CyberDeck Linux platform launcher startup.");
    }

    cyberdeck::settings::SettingsStore settings_store;
    cyberdeck::history::HistoryStore history_store;
    cyberdeck::deck::BookmarkStore bookmark_store;

    if (logging_ready) {
        settings_store.Initialize(cyberdeck::settings::SettingsStore::DefaultSettingsPath(), logger);
        history_store.Initialize(cyberdeck::history::HistoryStore::DefaultHistoryPath(), logger);
        bookmark_store.Initialize(cyberdeck::deck::BookmarkStore::DefaultBookmarksPath(), logger);
    }

    std::cout << Narrow(cyberdeck::common::AppName()) << " Linux platform build\n";
    std::cout << "version: " << Narrow(cyberdeck::common::AppVersion()) << '\n';
    std::cout << "target: CyberDeckBrowserLinux\n";
    std::cout << "data: " << cyberdeck::common::AppDataDirectory().string() << '\n';
    std::cout << "log: " << cyberdeck::common::Logger::DefaultLogPath().string() << '\n';
    std::cout << "browser shell: this binary is the core Linux diagnostics launcher\n";
    std::cout << "full browser: run ../build-linux-cef/cyberdeck-browser-cef or ./scripts/run_linux.sh\n";

    if (argc > 1 && argv[1] != nullptr) {
        const std::wstring input = cyberdeck::common::Utf8ToWide(argv[1]);
        const cyberdeck::browser::NormalizedNavigation normalized =
            cyberdeck::browser::NormalizeAddressBarInput(input);
        std::cout << "input: " << argv[1] << '\n';
        std::cout << "navigation: " << DecisionText(normalized.decision) << '\n';
        if (!normalized.target_url.empty()) {
            std::cout << "target: " << Narrow(normalized.target_url) << '\n';
        }
        if (!normalized.reason.empty()) {
            std::cout << "reason: " << Narrow(normalized.reason) << '\n';
        }
    }

    if (logging_ready) {
        logger.Info("CyberDeck Linux platform launcher shutdown.");
    }
    return EXIT_SUCCESS;
}
