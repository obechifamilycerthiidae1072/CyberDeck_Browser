#pragma once

#include <string>

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

}  // namespace cyberdeck::browser
