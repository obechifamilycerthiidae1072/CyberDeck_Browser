#pragma once

#include <windows.h>

namespace cyberdeck::ui {

struct Theme {
    static constexpr COLORREF black = RGB(0, 0, 0);
    static constexpr COLORREF green = RGB(0, 255, 0);
    static constexpr COLORREF yellow = RGB(255, 255, 0);
    static constexpr COLORREF red = RGB(255, 0, 0);
    static constexpr COLORREF dim_green = RGB(0, 128, 0);
    static constexpr COLORREF faint_green = RGB(0, 42, 0);
    static constexpr COLORREF dark_panel = RGB(3, 10, 6);
    static constexpr COLORREF darker_panel = RGB(1, 5, 3);
    static constexpr COLORREF red_dim = RGB(128, 0, 0);
};

HFONT CreateMonospaceFont(int height, int weight = FW_MEDIUM);
void FillRectColor(HDC dc, const RECT& rect, COLORREF color);
void FrameRectColor(HDC dc, const RECT& rect, COLORREF color, int width = 1);
void DrawGlowFrame(HDC dc, const RECT& rect, COLORREF color);
void DrawScanlineOverlay(HDC dc, const RECT& rect, int spacing = 4);
void DrawFlickerLines(HDC dc, const RECT& rect, int intensity, int frame);
void ApplyDarkWindowFrame(HWND hwnd);

}  // namespace cyberdeck::ui
