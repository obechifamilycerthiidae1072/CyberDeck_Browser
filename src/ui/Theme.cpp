#include "ui/Theme.h"

#include <dwmapi.h>

namespace cyberdeck::ui {

HFONT CreateMonospaceFont(int height, int weight) {
    LOGFONTW font{};
    font.lfHeight = height;
    font.lfWeight = weight;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_DEFAULT_PRECIS;
    font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    font.lfQuality = CLEARTYPE_QUALITY;
    font.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
    wcscpy_s(font.lfFaceName, L"Cascadia Mono");

    HFONT created = CreateFontIndirectW(&font);
    if (created != nullptr) {
        return created;
    }

    wcscpy_s(font.lfFaceName, L"Consolas");
    created = CreateFontIndirectW(&font);
    if (created != nullptr) {
        return created;
    }

    wcscpy_s(font.lfFaceName, L"Courier New");
    return CreateFontIndirectW(&font);
}

void FillRectColor(HDC dc, const RECT& rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    if (brush == nullptr) {
        return;
    }

    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void FrameRectColor(HDC dc, const RECT& rect, COLORREF color, int width) {
    HPEN pen = CreatePen(PS_SOLID, width, color);
    if (pen == nullptr) {
        return;
    }

    HGDIOBJ previous_pen = SelectObject(dc, pen);
    HGDIOBJ previous_brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(dc, previous_brush);
    SelectObject(dc, previous_pen);
    DeleteObject(pen);
}

void DrawGlowFrame(HDC dc, const RECT& rect, COLORREF color) {
    RECT glow = rect;
    InflateRect(&glow, -1, -1);
    FrameRectColor(dc, glow, Theme::faint_green, 3);
    FrameRectColor(dc, rect, color, 1);
}

void DrawScanlineOverlay(HDC dc, const RECT& rect, int spacing) {
    if (dc == nullptr || spacing <= 0 || rect.right <= rect.left || rect.bottom <= rect.top) {
        return;
    }

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 18, 0));
    if (pen == nullptr) {
        return;
    }

    HGDIOBJ previous_pen = SelectObject(dc, pen);
    for (int y = rect.top + 2; y < rect.bottom; y += spacing) {
        MoveToEx(dc, rect.left, y, nullptr);
        LineTo(dc, rect.right, y);
    }
    SelectObject(dc, previous_pen);
    DeleteObject(pen);
}

void DrawFlickerLines(HDC dc, const RECT& rect, int intensity, int frame) {
    if (dc == nullptr || intensity <= 0 || rect.right <= rect.left || rect.bottom <= rect.top) {
        return;
    }

    const int clamped_intensity = intensity > 2 ? 2 : intensity;
    const int spacing = clamped_intensity == 1 ? 72 : 46;
    const int offset = (frame * (clamped_intensity == 1 ? 3 : 7)) % spacing;
    const COLORREF color = clamped_intensity == 1 ? RGB(0, 34, 0) : RGB(0, 58, 0);

    HPEN pen = CreatePen(PS_SOLID, 1, color);
    if (pen == nullptr) {
        return;
    }

    HGDIOBJ previous_pen = SelectObject(dc, pen);
    for (int y = rect.top + offset; y < rect.bottom; y += spacing) {
        MoveToEx(dc, rect.left, y, nullptr);
        LineTo(dc, rect.right, y);
    }
    SelectObject(dc, previous_pen);
    DeleteObject(pen);
}

void ApplyDarkWindowFrame(HWND hwnd) {
    if (hwnd == nullptr) {
        return;
    }

    constexpr DWORD use_dark_mode = 20;
    BOOL enabled = TRUE;
    DwmSetWindowAttribute(hwnd, use_dark_mode, &enabled, sizeof(enabled));

    constexpr DWORD border_color_attribute = 34;
    COLORREF border = Theme::green;
    DwmSetWindowAttribute(hwnd, border_color_attribute, &border, sizeof(border));
}

}  // namespace cyberdeck::ui
