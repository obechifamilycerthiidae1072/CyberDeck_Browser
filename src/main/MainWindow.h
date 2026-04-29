#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <windows.h>

namespace cyberdeck::main {

struct ToolbarCallbacks {
    std::function<void()> back;
    std::function<void()> forward;
    std::function<void()> reload;
    std::function<void()> stop;
    std::function<void(bool)> terminal_mode;
    std::function<void(bool)> scanlines;
    std::function<void(bool)> glow;
    std::function<void(int)> flicker_intensity;
    std::function<void()> settings_panel;
    std::function<void(bool)> deck_space;
    std::function<void()> add_node;
    std::function<void(const std::wstring&)> navigate;
};

struct UiTabState {
    int id = 0;
    std::wstring title;
    std::wstring url;
    bool loading = false;
    bool url_committed = false;
    bool can_go_back = false;
    bool can_go_forward = false;
};

struct TabCallbacks {
    std::function<void()> new_tab;
    std::function<void(int)> activate_tab;
    std::function<void(int)> close_tab;
};

class MainWindow {
public:
    MainWindow() = default;
    ~MainWindow();

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

    bool Create(HINSTANCE instance, int show_command);
    RECT ContentBounds() const;
    void SetToolbarCallbacks(ToolbarCallbacks callbacks);
    void SetTabCallbacks(TabCallbacks callbacks);
    void SetTabs(const std::vector<UiTabState>& tabs, int active_tab_id);
    void SetUrlText(const std::wstring& url);
    void SetDownloadStatus(const std::wstring& status);
    void SetTerminalModeEnabled(bool enabled);
    void SetScanlinesEnabled(bool enabled);
    void SetGlowEnabled(bool enabled);
    void SetFlickerIntensity(int intensity);
    void SetDeckSpaceEnabled(bool enabled);
    void SetContentResizedCallback(std::function<void(const RECT&)> callback);
    HWND hwnd() const;

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
    static LRESULT CALLBACK UrlEditProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

    LRESULT HandleMessage(UINT message, WPARAM w_param, LPARAM l_param);
    void ApplyTabs(const std::vector<UiTabState>& tabs, int active_tab_id);
    bool CreateToolbarControls(HINSTANCE instance);
    void LayoutToolbarControls();
    void RebuildTabControls();
    void LayoutTabControls();
    void UpdateLoadingTimer();
    void NavigateFromEdit();
    void HandleTabStripClick(POINT point);
    void DrawToolbarButton(const DRAWITEMSTRUCT& draw_item);
    void Paint();
    void SetMinimumTrackSize(MINMAXINFO* minmax_info) const;
    void SetUrlTextInternal(const std::wstring& url);
    void MarkUrlEditedByUser(UINT message, WPARAM w_param);
    void ToggleScanlines();
    void ToggleGlow();
    void CycleFlickerIntensity();
    void InvalidateEffectControls();

    HWND hwnd_ = nullptr;
    HINSTANCE instance_ = nullptr;
    DWORD ui_thread_id_ = 0;
    HWND back_button_ = nullptr;
    HWND forward_button_ = nullptr;
    HWND reload_button_ = nullptr;
    HWND stop_button_ = nullptr;
    HWND terminal_mode_button_ = nullptr;
    HWND scanlines_button_ = nullptr;
    HWND glow_button_ = nullptr;
    HWND flicker_button_ = nullptr;
    HWND settings_button_ = nullptr;
    HWND deck_space_button_ = nullptr;
    HWND add_node_button_ = nullptr;
    HWND url_edit_ = nullptr;
    HWND go_button_ = nullptr;
    HWND new_tab_button_ = nullptr;
    std::vector<HWND> tab_buttons_;
    std::vector<HWND> tab_close_buttons_;
    HFONT toolbar_font_ = nullptr;
    HBRUSH edit_background_brush_ = nullptr;
    WNDPROC url_edit_proc_ = nullptr;
    bool url_edit_dirty_ = false;
    bool terminal_mode_enabled_ = false;
    bool scanlines_enabled_ = true;
    bool glow_enabled_ = true;
    int flicker_intensity_ = 0;
    int flicker_frame_ = 0;
    bool deck_space_enabled_ = false;
    ToolbarCallbacks toolbar_callbacks_;
    TabCallbacks tab_callbacks_;
    std::vector<UiTabState> tabs_;
    int active_tab_id_ = 0;
    std::mutex pending_tabs_mutex_;
    std::vector<UiTabState> pending_tabs_;
    int pending_active_tab_id_ = 0;
    std::wstring download_status_;
    std::mutex pending_download_status_mutex_;
    std::wstring pending_download_status_;
    int spinner_frame_ = 0;
    bool loading_timer_active_ = false;
    std::function<void(const RECT&)> content_resized_callback_;
};

}  // namespace cyberdeck::main
