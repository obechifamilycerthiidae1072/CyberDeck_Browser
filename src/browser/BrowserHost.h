#pragma once

#include "common/Logger.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>

namespace cyberdeck::browser {

struct BrowserTabState {
    int id = 0;
    std::wstring title;
    std::wstring url;
    bool loading = false;
    bool url_committed = false;
    bool can_go_back = false;
    bool can_go_forward = false;
};

struct DownloadStatus {
    int id = 0;
    std::wstring file_name;
    std::wstring target_path;
    std::wstring message;
    int percent_complete = -1;
    bool complete = false;
    bool canceled = false;
};

struct PermissionStatus {
    int tab_id = 0;
    std::wstring origin;
    std::wstring permission;
    std::wstring message;
    bool allowed = false;
};

class BrowserHost {
public:
    struct InitializeResult {
        bool success = false;
        bool should_exit = false;
        int exit_code = 0;
    };

    BrowserHost();
    ~BrowserHost();

    BrowserHost(const BrowserHost&) = delete;
    BrowserHost& operator=(const BrowserHost&) = delete;

    InitializeResult Initialize(HINSTANCE instance, common::Logger& logger);
    void SetTabsChangedCallback(std::function<void(const std::vector<BrowserTabState>&, int)> callback);
    void SetSuccessfulNavigationCallback(std::function<void(const BrowserTabState&)> callback);
    void SetDownloadStatusCallback(std::function<void(const DownloadStatus&)> callback);
    void SetPermissionStatusCallback(std::function<void(const PermissionStatus&)> callback);
    bool CreateInitialTab(HWND parent, const RECT& bounds, std::wstring_view initial_url = L"https://www.example.com");
    bool CreateTab(std::wstring_view initial_url = L"https://www.example.com");
    bool ActivateTab(int tab_id);
    bool CloseTab(int tab_id);
    void Resize(const RECT& bounds);
    void Navigate(std::wstring_view url);
    void GoBack();
    void GoForward();
    void Reload();
    void Stop();
    void SetTerminalModeEnabled(bool enabled);
    bool TerminalModeEnabled() const;
    void Shutdown();
    bool IsCefEnabled() const;
    std::wstring CefVersionText() const;
    int ActiveTabId() const;
    std::vector<BrowserTabState> Tabs() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace cyberdeck::browser
