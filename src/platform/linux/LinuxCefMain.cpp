#include "browser/BrowserTypes.h"
#include "browser/UrlNavigation.h"
#include "common/AppInfo.h"
#include "common/Logger.h"
#include "common/Platform.h"
#include "deck/BookmarkNode.h"
#include "deck/BookmarkStore.h"
#include "deck/FaviconStore.h"
#include "history/HistoryStore.h"
#include "render/DeckGeometry.h"
#include "render/DeckLayout.h"
#include "settings/SettingsStore.h"

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_command_line.h"
#include "include/cef_display_handler.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_load_handler.h"
#include "include/cef_request_handler.h"
#include "include/cef_render_handler.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace {

using cyberdeck::browser::BrowserTabState;

constexpr int kInitialWidth = 1280;
constexpr int kInitialHeight = 800;
constexpr int kMinimumWidth = 1100;
constexpr int kMinimumHeight = 700;
constexpr int kToolbarHeight = 64;
constexpr int kTabStripHeight = 46;
constexpr int kToolbarMargin = 10;
constexpr int kToolbarGap = 7;
constexpr int kButtonWidth = 72;
constexpr int kTerminalButtonWidth = 88;
constexpr int kEffectButtonWidth = 70;
constexpr int kSettingsButtonWidth = 62;
constexpr int kDeckButtonWidth = 72;
constexpr int kAddNodeButtonWidth = 112;
constexpr int kGoButtonWidth = 62;
constexpr int kControlHeight = 40;
constexpr int kUrlMinimumWidth = 96;
constexpr std::size_t kMaxUrlTextLength = 8192;
constexpr int kMaxPaintDimension = 8192;
constexpr std::size_t kMaxPaintPixels =
    static_cast<std::size_t>(kMaxPaintDimension) * static_cast<std::size_t>(kMaxPaintDimension);
constexpr int kTabWidth = 240;
constexpr int kTabHeight = 34;
constexpr int kNewTabWidth = 44;
constexpr int kTabTop = kToolbarHeight + 6;
constexpr int kContentTop = kToolbarHeight + kTabStripHeight;
constexpr int kGlyphColumns = 5;
constexpr int kGlyphRows = 7;
constexpr int kGlyphScaleX = 2;
constexpr int kGlyphScaleY = 3;
constexpr int kGlyphSpacing = 1;
constexpr int kTextPixelHeight = kGlyphRows * kGlyphScaleY;
constexpr int kApproxCharWidth = (kGlyphColumns + kGlyphSpacing) * kGlyphScaleX;
constexpr int kTextInset = 10;

constexpr unsigned long kBlack = 0x000000;
constexpr unsigned long kGreen = 0x00ff00;
constexpr unsigned long kDimGreen = 0x008000;
constexpr unsigned long kFaintGreen = 0x002a00;
constexpr unsigned long kDarkPanel = 0x030a06;
constexpr unsigned long kDarkerPanel = 0x010503;
constexpr unsigned long kYellow = 0xffff00;
constexpr unsigned long kRed = 0xff0000;
constexpr unsigned long kRedDim = 0x800000;
constexpr unsigned long kCyanDim = 0x004040;

struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

using GlyphRows = std::array<std::uint8_t, kGlyphRows>;

// Built-in chrome glyphs keep the Linux shell readable without distro font dependencies.
GlyphRows GlyphFor(char raw) {
    char ch = raw;
    if (ch >= 'a' && ch <= 'z') {
        ch = static_cast<char>(ch - 'a' + 'A');
    }

    switch (ch) {
        case ' ': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        case '0': return {0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e};
        case '1': return {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e};
        case '2': return {0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f};
        case '3': return {0x1e, 0x01, 0x01, 0x0e, 0x01, 0x01, 0x1e};
        case '4': return {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02};
        case '5': return {0x1f, 0x10, 0x10, 0x1e, 0x01, 0x01, 0x1e};
        case '6': return {0x0e, 0x10, 0x10, 0x1e, 0x11, 0x11, 0x0e};
        case '7': return {0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08};
        case '8': return {0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e};
        case '9': return {0x0e, 0x11, 0x11, 0x0f, 0x01, 0x01, 0x0e};
        case 'A': return {0x0e, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11};
        case 'B': return {0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e};
        case 'C': return {0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e};
        case 'D': return {0x1e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1e};
        case 'E': return {0x1f, 0x10, 0x10, 0x1e, 0x10, 0x10, 0x1f};
        case 'F': return {0x1f, 0x10, 0x10, 0x1e, 0x10, 0x10, 0x10};
        case 'G': return {0x0e, 0x11, 0x10, 0x13, 0x11, 0x11, 0x0e};
        case 'H': return {0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11};
        case 'I': return {0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1f};
        case 'J': return {0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0e};
        case 'K': return {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11};
        case 'L': return {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f};
        case 'M': return {0x11, 0x1b, 0x15, 0x15, 0x11, 0x11, 0x11};
        case 'N': return {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11};
        case 'O': return {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e};
        case 'P': return {0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10};
        case 'Q': return {0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d};
        case 'R': return {0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11};
        case 'S': return {0x0f, 0x10, 0x10, 0x0e, 0x01, 0x01, 0x1e};
        case 'T': return {0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
        case 'U': return {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e};
        case 'V': return {0x11, 0x11, 0x11, 0x11, 0x0a, 0x0a, 0x04};
        case 'W': return {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0a};
        case 'X': return {0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11};
        case 'Y': return {0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04};
        case 'Z': return {0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f};
        case '.': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c};
        case ',': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x08};
        case ':': return {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x0c, 0x00};
        case ';': return {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x08, 0x10};
        case '-': return {0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00};
        case '_': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f};
        case '/': return {0x01, 0x02, 0x02, 0x04, 0x08, 0x08, 0x10};
        case '\\': return {0x10, 0x08, 0x08, 0x04, 0x02, 0x02, 0x01};
        case '?': return {0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04};
        case '!': return {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04};
        case '+': return {0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00};
        case '*': return {0x00, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x00};
        case '=': return {0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00};
        case '<': return {0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02};
        case '>': return {0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08};
        case '(': return {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02};
        case ')': return {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08};
        case '[': return {0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e};
        case ']': return {0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e};
        case '{': return {0x06, 0x08, 0x08, 0x10, 0x08, 0x08, 0x06};
        case '}': return {0x0c, 0x02, 0x02, 0x01, 0x02, 0x02, 0x0c};
        case '@': return {0x0e, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0e};
        case '#': return {0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a};
        case '%': return {0x19, 0x1a, 0x02, 0x04, 0x08, 0x0b, 0x13};
        case '&': return {0x0c, 0x12, 0x14, 0x08, 0x15, 0x12, 0x0d};
        case '\'': return {0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00};
        case '"': return {0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00};
        case '`': return {0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
        case '~': return {0x00, 0x00, 0x00, 0x0d, 0x16, 0x00, 0x00};
        case '$': return {0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04};
        case '^': return {0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00};
        case '|': return {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
        default: return {0x1f, 0x11, 0x05, 0x02, 0x04, 0x00, 0x04};
    }
}

enum class ControlId {
    Back,
    Forward,
    Reload,
    Stop,
    Terminal,
    Scanlines,
    Glow,
    Flicker,
    Settings,
    Deck,
    AddNode,
    Url,
    Go,
    NewTab,
    Tab,
};

struct ControlRect {
    ControlId id = ControlId::Back;
    Rect rect;
};

struct DeckHit {
    std::size_t index = 0;
    Rect rect;
    float depth = 0.0f;
};

struct ProjectedPoint {
    int x = 0;
    int y = 0;
    float depth = 0.0f;
};

struct ProjectedFace {
    std::vector<XPoint> points;
    float depth = 0.0f;
    unsigned long fill = kBlack;
    unsigned long line = kGreen;
    bool selected = false;
};

std::string ToUtf8(std::wstring_view value) {
    return cyberdeck::common::WideToUtf8(value);
}

std::wstring ToWide(std::string_view value) {
    return cyberdeck::common::Utf8ToWide(value);
}

std::string NarrowAscii(std::wstring_view value) {
    std::string output;
    output.reserve(value.size());
    for (wchar_t ch : value) {
        output.push_back(ch <= 0x7f ? static_cast<char>(ch) : '?');
    }
    return output;
}

float DegreesToRadians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

cyberdeck::render::Vec3 RotateX(cyberdeck::render::Vec3 value, float degrees) {
    const float radians = DegreesToRadians(degrees);
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    return {value.x, value.y * cosine - value.z * sine, value.y * sine + value.z * cosine};
}

cyberdeck::render::Vec3 RotateY(cyberdeck::render::Vec3 value, float degrees) {
    const float radians = DegreesToRadians(degrees);
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    return {value.x * cosine + value.z * sine, value.y, -value.x * sine + value.z * cosine};
}

cyberdeck::render::Vec3 RotateZ(cyberdeck::render::Vec3 value, float degrees) {
    const float radians = DegreesToRadians(degrees);
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    return {value.x * cosine - value.y * sine, value.x * sine + value.y * cosine, value.z};
}

unsigned long ScaleColor(unsigned long color, float factor) {
    const auto scale = [factor](unsigned long channel) {
        return static_cast<unsigned long>(std::clamp(static_cast<int>(static_cast<float>(channel) * factor), 0, 255));
    };
    const unsigned long red = scale((color >> 16) & 0xff);
    const unsigned long green = scale((color >> 8) & 0xff);
    const unsigned long blue = scale(color & 0xff);
    return (red << 16) | (green << 8) | blue;
}

cyberdeck::render::DeckShape ShapeFromNode(
    const cyberdeck::deck::BookmarkNode& node,
    cyberdeck::render::DeckLayoutMode layout_mode) {
    if (layout_mode == cyberdeck::render::DeckLayoutMode::GridDeck) {
        return cyberdeck::render::DeckShape::BeveledTile;
    }
    if (layout_mode == cyberdeck::render::DeckLayoutMode::CubeOrbit &&
        node.shape_type == cyberdeck::deck::BookmarkNodeShapeType::Hex) {
        return cyberdeck::render::DeckShape::Cube;
    }
    switch (node.shape_type) {
        case cyberdeck::deck::BookmarkNodeShapeType::Hex:
            return cyberdeck::render::DeckShape::HexPrism;
        case cyberdeck::deck::BookmarkNodeShapeType::Cube:
            return cyberdeck::render::DeckShape::Cube;
        case cyberdeck::deck::BookmarkNodeShapeType::Panel:
            return cyberdeck::render::DeckShape::BeveledTile;
    }
    return cyberdeck::render::DeckShape::HexPrism;
}

cyberdeck::render::DeckMesh MeshForShape(cyberdeck::render::DeckShape shape) {
    switch (shape) {
        case cyberdeck::render::DeckShape::HexPrism:
            return cyberdeck::render::GenerateHexPrismMesh(0.82f, 0.48f);
        case cyberdeck::render::DeckShape::Cube:
            return cyberdeck::render::GenerateCubeMesh(1.0f);
        case cyberdeck::render::DeckShape::BeveledTile:
            return cyberdeck::render::GenerateBeveledTileMesh();
    }
    return cyberdeck::render::GenerateHexPrismMesh(0.82f, 0.48f);
}

bool StartsWith(std::wstring_view value, std::wstring_view prefix) {
    return value.size() >= prefix.size() && value.substr(0, prefix.size()) == prefix;
}

bool Contains(const Rect& rect, int x, int y) {
    return x >= rect.x && y >= rect.y && x < rect.x + rect.width && y < rect.y + rect.height;
}

std::string Compact(std::string value, std::size_t max_chars) {
    if (max_chars < 4 || value.size() <= max_chars) {
        return value;
    }
    value.resize(max_chars - 3);
    value += "...";
    return value;
}

std::string CompactForRect(std::string value, const Rect& rect, int padding = 12) {
    const int usable_width = std::max(0, rect.width - padding);
    const std::size_t max_chars = static_cast<std::size_t>(std::max(1, usable_width / kApproxCharWidth));
    return Compact(std::move(value), max_chars);
}

std::string StartupUrl(int argc, char** argv) {
    std::optional<std::string> startup_arg;
    for (int index = 1; index < argc; ++index) {
        if (argv[index] == nullptr) {
            continue;
        }
        const std::string candidate = argv[index];
        if (candidate.rfind("--", 0) == 0) {
            continue;
        }
        if (candidate.size() > kMaxUrlTextLength) {
            std::cerr << "Ignoring startup URL because it is too long.\n";
            return "https://www.example.com";
        }
        startup_arg = candidate;
        break;
    }

    if (!startup_arg) {
        return "https://www.example.com";
    }

    const cyberdeck::browser::NormalizedNavigation normalized =
        cyberdeck::browser::NormalizeAddressBarInput(ToWide(*startup_arg));
    if (normalized.decision == cyberdeck::browser::NavigationDecision::kNavigate &&
        !normalized.target_url.empty()) {
        return ToUtf8(normalized.target_url);
    }

    std::cerr << "Ignoring startup URL: " << *startup_arg << '\n';
    if (!normalized.reason.empty()) {
        std::cerr << "Reason: " << ToUtf8(normalized.reason) << '\n';
    }
    return "https://www.example.com";
}

bool HasSwitch(int argc, char** argv, const std::string& name) {
    for (int index = 1; index < argc; ++index) {
        if (argv[index] != nullptr && name == argv[index]) {
            return true;
        }
    }
    return false;
}

bool IsWsl() {
    FILE* file = std::fopen("/proc/version", "r");
    if (file == nullptr) {
        return false;
    }

    char buffer[512]{};
    const std::size_t read = std::fread(buffer, 1, sizeof(buffer) - 1, file);
    std::fclose(file);
    std::string version(buffer, read);
    std::transform(version.begin(), version.end(), version.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return version.find("microsoft") != std::string::npos;
}

std::wstring TerminalModeInjectionScript() {
    return LR"JS((function(){
  try {
    var id = 'cyberdeck-terminal-mode-style';
    var existing = document.getElementById(id);
    if (existing) { existing.remove(); }
    var style = document.createElement('style');
    style.id = id;
    style.textContent = `
      html, body {
        background: #000000 !important;
        background-color: #000000 !important;
        color: #00ff00 !important;
        color-scheme: dark !important;
        font-family: "Cascadia Mono", Consolas, "Courier New", monospace !important;
        text-shadow: 0 0 4px rgba(0, 255, 0, 0.45) !important;
      }
      body *:not(img):not(video):not(canvas):not(svg):not(path):not(circle):not(rect):not(line):not(polyline):not(polygon) {
        background-color: #000000 !important;
        color: #00ff00 !important;
        border-color: #008000 !important;
        font-family: "Cascadia Mono", Consolas, "Courier New", monospace !important;
      }
      body,
      main,
      section,
      article,
      aside,
      nav,
      header,
      footer,
      form,
      dialog,
      menu,
      div,
      span,
      p,
      ul,
      ol,
      li,
      table,
      thead,
      tbody,
      tr,
      td,
      th,
      [role],
      [class],
      [data-testid],
      [style*="background"],
      [style*="background-color"] {
        background: #000000 !important;
        background-color: #000000 !important;
        background-image: none !important;
      }
      a, a *, [role="link"], [role="link"] * {
        color: #ffff00 !important;
        text-shadow: 0 0 4px rgba(255, 255, 0, 0.4) !important;
      }
      strong, b, em, mark, h1, h2, h3, h4, h5, h6 {
        color: #ff0000 !important;
        text-shadow: 0 0 5px rgba(255, 0, 0, 0.45) !important;
      }
      input, textarea, select, button {
        background: #000000 !important;
        background-color: #000000 !important;
        background-image: none !important;
        color: #00ff00 !important;
        caret-color: #ffff00 !important;
        border: 1px solid #008000 !important;
        box-shadow: 0 0 6px rgba(0, 255, 0, 0.2) !important;
      }
      pre, code, kbd, samp {
        background: #001006 !important;
        background-color: #001006 !important;
        color: #ffff00 !important;
      }
      *::before, *::after {
        color: #00ff00 !important;
        border-color: #008000 !important;
      }
      img, video, canvas, svg {
        filter: brightness(0.78) contrast(1.1) saturate(0.75) !important;
      }
      ::selection {
        background: #ffff00 !important;
        color: #000000 !important;
      }
    `;
    (document.head || document.documentElement).appendChild(style);
    document.documentElement.dataset.cyberdeckTerminalMode = 'enabled';
  } catch (error) {
    console.warn('CyberDeck Terminal Mode injection failed', error);
  }
})();)JS";
}

std::wstring TerminalModeRemovalScript() {
    return LR"JS((function(){
  try {
    var existing = document.getElementById('cyberdeck-terminal-mode-style');
    if (existing) { existing.remove(); }
    if (document.documentElement && document.documentElement.dataset) {
      delete document.documentElement.dataset.cyberdeckTerminalMode;
    }
  } catch (error) {
    console.warn('CyberDeck Terminal Mode removal failed', error);
  }
})();)JS";
}

std::string ErrorName(int code) {
    switch (code) {
        case -3:
            return "ERR_ABORTED";
        case -105:
            return "ERR_NAME_NOT_RESOLVED";
        case -106:
            return "ERR_INTERNET_DISCONNECTED";
        case -118:
            return "ERR_CONNECTION_TIMED_OUT";
        case -200:
            return "ERR_CERT_COMMON_NAME_INVALID";
        case -201:
            return "ERR_CERT_DATE_INVALID";
        case -202:
            return "ERR_CERT_AUTHORITY_INVALID";
        default:
            return "CEF_ERROR_" + std::to_string(code);
    }
}

class LinuxCyberDeckShell;

class LinuxBrowserClient final
    : public CefClient,
      public CefLifeSpanHandler,
      public CefLoadHandler,
      public CefDisplayHandler,
      public CefRequestHandler,
      public CefRenderHandler {
public:
    explicit LinuxBrowserClient(LinuxCyberDeckShell& shell) : shell_(shell) {}

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
        return this;
    }

    CefRefPtr<CefLoadHandler> GetLoadHandler() override {
        return this;
    }

    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
        return this;
    }

    CefRefPtr<CefRequestHandler> GetRequestHandler() override {
        return this;
    }

    CefRefPtr<CefRenderHandler> GetRenderHandler() override {
        return this;
    }

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
    void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool is_loading, bool can_go_back, bool can_go_forward) override;
    void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) override;
    void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int http_status_code) override;
    void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode error_code,
        const CefString& error_text,
        const CefString& failed_url) override;
    bool OnBeforeBrowse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool user_gesture,
        bool is_redirect) override;
    bool OnOpenURLFromTab(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& target_url,
        cef_window_open_disposition_t target_disposition,
        bool user_gesture) override;
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int view_x, int view_y, int& screen_x, int& screen_y) override;
    void OnPaint(
        CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirty_rects,
        const void* buffer,
        int width,
        int height) override;

private:
    LinuxCyberDeckShell& shell_;

    IMPLEMENT_REFCOUNTING(LinuxBrowserClient);
};

class LinuxBrowserApp final : public CefApp, public CefBrowserProcessHandler {
public:
    LinuxBrowserApp(bool enable_gpu) : enable_gpu_(enable_gpu) {}

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override {
        if (!process_type.empty()) {
            return;
        }

        command_line->AppendSwitch("no-sandbox");
        command_line->AppendSwitch("disable-gpu-sandbox");
        command_line->AppendSwitch("disable-features=AutofillActorMode,GlicActorUi,LensOverlay");

        if (!enable_gpu_) {
            command_line->AppendSwitch("disable-gpu");
            command_line->AppendSwitch("disable-gpu-compositing");
            command_line->AppendSwitch("disable-gpu-rasterization");
        }
    }

private:
    bool enable_gpu_ = false;

    IMPLEMENT_REFCOUNTING(LinuxBrowserApp);
};

class LinuxCyberDeckShell {
public:
    LinuxCyberDeckShell(std::string startup_url, bool smoke_test, bool start_deck)
        : startup_url_(std::move(startup_url)), smoke_test_(smoke_test), start_deck_(start_deck) {}

    bool Initialize() {
        logging_ready_ = logger_.Initialize(cyberdeck::common::Logger::DefaultLogPath());
        if (logging_ready_) {
            logger_.Info("CyberDeck Linux CEF shell startup.");
        }

        settings_ready_ = settings_store_.Initialize(cyberdeck::settings::SettingsStore::DefaultSettingsPath(), logger_);
        if (settings_ready_) {
            settings_ = settings_store_.Settings();
        }
        history_store_.Initialize(cyberdeck::history::HistoryStore::DefaultHistoryPath(), logger_);
        bookmark_store_.Initialize(cyberdeck::deck::BookmarkStore::DefaultBookmarksPath(), logger_);

        terminal_mode_enabled_ = settings_.terminal_mode_enabled;
        scanlines_enabled_ = settings_.scanlines_enabled;
        glow_enabled_ = settings_.glow_enabled;
        flicker_intensity_ = std::clamp(settings_.flicker_intensity, 0, 2);
        deck_layout_mode_ = cyberdeck::render::DeckLayoutModeFromString(NarrowAscii(settings_.deck_layout_mode));

        XInitThreads();
        display_ = XOpenDisplay(nullptr);
        if (display_ == nullptr) {
            std::cerr << "Unable to open X display. Is WSLg/X11 running?\n";
            return false;
        }

        screen_ = DefaultScreen(display_);
        window_ = XCreateSimpleWindow(
            display_,
            RootWindow(display_, screen_),
            80,
            80,
            kInitialWidth,
            kInitialHeight,
            1,
            kGreen,
            kBlack);
        width_ = kInitialWidth;
        height_ = kInitialHeight;

        XStoreName(display_, window_, ToUtf8(cyberdeck::common::AppName()).c_str());
        XSelectInput(
            display_,
            window_,
            ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask |
                PointerMotionMask | KeyPressMask | KeyReleaseMask | FocusChangeMask);

        XSizeHints hints{};
        hints.flags = PMinSize;
        hints.min_width = kMinimumWidth;
        hints.min_height = kMinimumHeight;
        XSetWMNormalHints(display_, window_, &hints);

        wm_delete_ = XInternAtom(display_, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display_, window_, &wm_delete_, 1);

        gc_ = XCreateGC(display_, window_, 0, nullptr);

        XMapWindow(display_, window_);
        XFlush(display_);

        current_url_ = ToWide(startup_url_);
        url_text_ = startup_url_;
        deck_space_enabled_ = start_deck_;
        status_text_ = deck_space_enabled_ ? "DECK SPACE ACTIVE" : "LINUX SHELL READY";

        client_ = new LinuxBrowserClient(*this);
        CefWindowInfo window_info;
        window_info.SetAsWindowless(window_);

        CefBrowserSettings browser_settings;
        browser_settings.background_color = CefColorSetARGB(255, 0, 0, 0);

        CefBrowserHost::CreateBrowser(
            window_info,
            client_,
            CefString(startup_url_),
            browser_settings,
            nullptr,
            nullptr);

        return true;
    }

    int Run() {
        const auto start = std::chrono::steady_clock::now();
        auto last_tick = start;
        while (running_) {
            while (display_ != nullptr && XPending(display_) > 0) {
                XEvent event{};
                XNextEvent(display_, &event);
                HandleEvent(event);
            }

            CefDoMessageLoopWork();

            const auto now = std::chrono::steady_clock::now();
            if (now - last_tick > std::chrono::milliseconds(160)) {
                ++animation_frame_;
                if (flicker_intensity_ > 0 || deck_space_enabled_) {
                    Draw();
                }
                last_tick = now;
            }

            if (smoke_test_ && browser_created_) {
                const bool loaded_or_timed_out = !loading_ || now - start > std::chrono::seconds(6);
                const bool painted_or_timed_out = paint_count_ > 0 || now - start > std::chrono::seconds(6);
                if (loaded_or_timed_out && painted_or_timed_out) {
                    Close();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        ShutdownX11();
        if (logging_ready_) {
            logger_.Info("CyberDeck Linux CEF shell shutdown.");
        }
        return EXIT_SUCCESS;
    }

    void SetBrowser(CefRefPtr<CefBrowser> browser) {
        browser_ = browser;
        browser_created_ = browser_ != nullptr;
        ResizeBrowser();
        Draw();
    }

    void BrowserClosed(CefRefPtr<CefBrowser>) {
        browser_ = nullptr;
        running_ = false;
    }

    void UpdateTitle(const std::wstring& title) {
        current_title_ = title.empty() ? L"New Tab" : title;
        if (display_ != nullptr && window_ != 0) {
            const std::string app_title =
                ToUtf8(cyberdeck::common::AppName()) + " - " + ToUtf8(current_title_);
            XStoreName(display_, window_, app_title.c_str());
        }
        Draw();
    }

    void UpdateAddress(const std::wstring& url) {
        current_url_ = url;
        if (!url_focused_) {
            url_text_ = ToUtf8(url);
        }
        Draw();
    }

    void UpdateLoading(bool loading, bool can_go_back, bool can_go_forward) {
        loading_ = loading;
        can_go_back_ = can_go_back;
        can_go_forward_ = can_go_forward;
        Draw();
    }

    void LoadStarted(const std::wstring& url) {
        if (!url.empty()) {
            current_url_ = url;
            if (!url_focused_) {
                url_text_ = ToUtf8(url);
            }
        }
        loading_ = true;
        status_text_ = "LOADING";
        Draw();
    }

    void LoadEnded(int http_status_code) {
        loading_ = false;
        status_text_ = "HTTP " + std::to_string(http_status_code);
        if (terminal_mode_enabled_) {
            ApplyTerminalMode();
        }
        if (!current_url_.empty() && !StartsWith(current_url_, L"about:")) {
            history_store_.RecordVisit(current_title_, current_url_);
        }
        Draw();
    }

    void LoadFailed(int error_code, const std::wstring& failed_url) {
        if (error_code == -3) {
            return;
        }
        loading_ = false;
        current_url_ = failed_url;
        url_text_ = ToUtf8(failed_url);
        status_text_ = "LOAD FAILED: " + ErrorName(error_code);
        Draw();
    }

    bool HandleProtocolNavigation(std::wstring_view url, std::string_view source, bool user_gesture) {
        const cyberdeck::browser::ProtocolDecision decision =
            cyberdeck::browser::ClassifyNavigationProtocol(url);
        if (decision.action == cyberdeck::browser::ProtocolAction::kAllow) {
            return false;
        }

        const std::string scheme = ToUtf8(decision.scheme);
        const std::string compact_url = Compact(ToUtf8(url), 300);
        if (decision.action == cyberdeck::browser::ProtocolAction::kBlock) {
            status_text_ = scheme.empty() ? "NAVIGATION BLOCKED" : "NAVIGATION BLOCKED: " + scheme;
            if (logging_ready_) {
                logger_.Error(
                    "Blocked protocol navigation from " + std::string(source) +
                    ": scheme=" + scheme +
                    " url=" + compact_url +
                    " reason=" + ToUtf8(decision.reason) + ".");
            }
            Draw();
            return true;
        }

        status_text_ = user_gesture ? "EXTERNAL PROTOCOL BLOCKED: " + scheme
                                    : "BACKGROUND PROTOCOL BLOCKED: " + scheme;
        if (logging_ready_) {
            logger_.Error(
                "Blocked external protocol on Linux from " + std::string(source) +
                ": scheme=" + scheme +
                " url=" + compact_url +
                " user_gesture=" + std::string(user_gesture ? "true" : "false") + ".");
        }
        Draw();
        return true;
    }

    Rect WebRect() const {
        return {
            0,
            kContentTop,
            std::max(1, width_),
            std::max(1, height_ - kContentTop),
        };
    }

    void ViewRect(CefRect& rect) const {
        const Rect web = WebRect();
        rect = CefRect(0, 0, web.width, web.height);
    }

    bool ScreenPoint(int view_x, int view_y, int& screen_x, int& screen_y) const {
        if (display_ == nullptr || window_ == 0) {
            return false;
        }
        Window child{};
        int root_x = 0;
        int root_y = 0;
        XTranslateCoordinates(display_, window_, RootWindow(display_, screen_), view_x, view_y + kContentTop, &root_x, &root_y, &child);
        screen_x = root_x;
        screen_y = root_y;
        return true;
    }

    void BrowserPaint(const void* buffer, int width, int height) {
        if (buffer == nullptr || width <= 0 || height <= 0) {
            return;
        }
        if (width > kMaxPaintDimension || height > kMaxPaintDimension) {
            if (logging_ready_) {
                logger_.Error(
                    "Rejected oversized CEF paint buffer: " + std::to_string(width) +
                    "x" + std::to_string(height) + ".");
            }
            return;
        }

        const std::size_t count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        if (count > kMaxPaintPixels) {
            if (logging_ready_) {
                logger_.Error(
                    "Rejected oversized CEF paint pixel count: " + std::to_string(count) + ".");
            }
            return;
        }

        try {
            web_pixels_.resize(count);
        } catch (const std::bad_alloc&) {
            web_pixels_.clear();
            web_width_ = 0;
            web_height_ = 0;
            if (logging_ready_) {
                logger_.Error("Rejected CEF paint buffer after allocation failure.");
            }
            return;
        }
        std::memcpy(web_pixels_.data(), buffer, count * sizeof(std::uint32_t));
        web_width_ = width;
        web_height_ = height;
        ++paint_count_;
        if (!deck_space_enabled_ && !settings_panel_enabled_) {
            Draw();
        }
    }

private:
    std::vector<ControlRect> BuildControls() const {
        std::vector<ControlRect> controls;
        const int y = kToolbarMargin + 1;
        int x = kToolbarMargin;

        auto push = [&controls, &x, y](ControlId id, int width) {
            controls.push_back({id, {x, y, width, kControlHeight}});
            x += width + kToolbarGap;
        };

        push(ControlId::Back, kButtonWidth);
        push(ControlId::Forward, kButtonWidth);
        push(ControlId::Reload, kButtonWidth);
        push(ControlId::Stop, kButtonWidth);
        push(ControlId::Terminal, kTerminalButtonWidth);
        push(ControlId::Scanlines, kEffectButtonWidth);
        push(ControlId::Glow, kEffectButtonWidth);
        push(ControlId::Flicker, kEffectButtonWidth);
        push(ControlId::Settings, kSettingsButtonWidth);
        push(ControlId::Deck, kDeckButtonWidth);
        push(ControlId::AddNode, kAddNodeButtonWidth);

        const int go_x = std::max(x + kUrlMinimumWidth + kToolbarGap, width_ - kToolbarMargin - kGoButtonWidth);
        const int edit_width = std::max(kUrlMinimumWidth, go_x - x - kToolbarGap);
        controls.push_back({ControlId::Url, {x, y, edit_width, kControlHeight}});
        controls.push_back({ControlId::Go, {x + edit_width + kToolbarGap, y, kGoButtonWidth, kControlHeight}});
        controls.push_back({ControlId::Tab, {kToolbarMargin, kTabTop, kTabWidth, kTabHeight}});
        controls.push_back({ControlId::NewTab, {kToolbarMargin + kTabWidth + kToolbarGap, kTabTop, kNewTabWidth, kTabHeight}});
        return controls;
    }

    std::optional<ControlId> HitControl(int x, int y) const {
        for (const ControlRect& control : BuildControls()) {
            if (Contains(control.rect, x, y)) {
                return control.id;
            }
        }
        return std::nullopt;
    }

    Rect ControlBounds(ControlId id) const {
        for (const ControlRect& control : BuildControls()) {
            if (control.id == id) {
                return control.rect;
            }
        }
        return {};
    }

    int TextWidth(const std::string& value) const {
        if (value.empty()) {
            return 0;
        }
        return static_cast<int>(value.size()) * kApproxCharWidth - (kGlyphSpacing * kGlyphScaleX);
    }

    int TextBaseline(const Rect& rect) const {
        return rect.y + std::max(0, rect.height - kTextPixelHeight) / 2 + kTextPixelHeight;
    }

    void HandleEvent(const XEvent& event) {
        switch (event.type) {
            case ClientMessage:
                if (static_cast<Atom>(event.xclient.data.l[0]) == wm_delete_) {
                    Close();
                }
                break;
            case ConfigureNotify:
                width_ = std::max(1, event.xconfigure.width);
                height_ = std::max(1, event.xconfigure.height);
                ResizeBrowser();
                Draw();
                break;
            case Expose:
                Draw();
                break;
            case FocusOut:
                url_focused_ = false;
                Draw();
                break;
            case ButtonPress:
                HandleButtonPress(event.xbutton);
                break;
            case ButtonRelease:
                HandleButtonRelease(event.xbutton);
                break;
            case MotionNotify:
                HandleMotion(event.xmotion);
                break;
            case KeyPress:
                HandleKeyPress(event.xkey);
                break;
            case KeyRelease:
                HandleKeyRelease(event.xkey);
                break;
            default:
                break;
        }
    }

    void HandleButtonPress(const XButtonEvent& event) {
        if (const auto control = HitControl(event.x, event.y)) {
            HandleControlClick(*control);
            return;
        }

        url_focused_ = false;
        if (deck_space_enabled_) {
            HandleDeckClick(event.x, event.y);
            return;
        }

        const Rect web = WebRect();
        if (browser_ && Contains(web, event.x, event.y)) {
            CefMouseEvent mouse_event;
            mouse_event.x = event.x - web.x;
            mouse_event.y = event.y - web.y;
            mouse_event.modifiers = EventModifiers(event.state);
            browser_->GetHost()->SetFocus(true);
            browser_->GetHost()->SendMouseClickEvent(mouse_event, MouseButton(event.button), false, 1);
        }
    }

    void HandleButtonRelease(const XButtonEvent& event) {
        if (deck_space_enabled_) {
            return;
        }

        const Rect web = WebRect();
        if (browser_ && Contains(web, event.x, event.y)) {
            CefMouseEvent mouse_event;
            mouse_event.x = event.x - web.x;
            mouse_event.y = event.y - web.y;
            mouse_event.modifiers = EventModifiers(event.state);
            browser_->GetHost()->SendMouseClickEvent(mouse_event, MouseButton(event.button), true, 1);
        }
    }

    void HandleMotion(const XMotionEvent& event) {
        if (deck_space_enabled_) {
            return;
        }

        const Rect web = WebRect();
        if (browser_ && Contains(web, event.x, event.y)) {
            CefMouseEvent mouse_event;
            mouse_event.x = event.x - web.x;
            mouse_event.y = event.y - web.y;
            mouse_event.modifiers = EventModifiers(event.state);
            browser_->GetHost()->SendMouseMoveEvent(mouse_event, false);
        }
    }

    void HandleKeyPress(const XKeyEvent& event) {
        KeySym key = NoSymbol;
        char buffer[64]{};
        XKeyEvent mutable_event = event;
        const int length = XLookupString(&mutable_event, buffer, sizeof(buffer) - 1, &key, nullptr);

        if (url_focused_) {
            if (key == XK_Return || key == XK_KP_Enter) {
                NavigateFromUrlText();
                return;
            }
            if (key == XK_Escape) {
                url_focused_ = false;
                url_text_ = ToUtf8(current_url_);
                Draw();
                return;
            }
            if (key == XK_BackSpace) {
                if (!url_text_.empty()) {
                    url_text_.pop_back();
                }
                Draw();
                return;
            }
            if (length > 0) {
                for (int index = 0; index < length; ++index) {
                    const unsigned char ch = static_cast<unsigned char>(buffer[index]);
                    if (ch >= 32 && ch < 127) {
                        if (url_text_.size() < kMaxUrlTextLength) {
                            url_text_.push_back(static_cast<char>(ch));
                        } else {
                            status_text_ = "ADDRESS TOO LONG";
                        }
                    }
                }
                Draw();
            }
            return;
        }

        if (deck_space_enabled_) {
            HandleDeckKey(key);
            return;
        }

        if (key == XK_Escape && settings_panel_enabled_) {
            settings_panel_enabled_ = false;
            Draw();
            return;
        }

        if (browser_) {
            SendKeyEvent(event, key, buffer, length, KEYEVENT_RAWKEYDOWN);
            if (length > 0) {
                SendKeyEvent(event, key, buffer, length, KEYEVENT_CHAR);
            }
        }
    }

    void HandleKeyRelease(const XKeyEvent& event) {
        if (url_focused_ || deck_space_enabled_ || !browser_) {
            return;
        }

        KeySym key = NoSymbol;
        char buffer[8]{};
        XKeyEvent mutable_event = event;
        const int length = XLookupString(&mutable_event, buffer, sizeof(buffer) - 1, &key, nullptr);
        SendKeyEvent(event, key, buffer, length, KEYEVENT_KEYUP);
    }

    void SendKeyEvent(
        const XKeyEvent& event,
        KeySym key,
        const char* text,
        int text_length,
        cef_key_event_type_t type) {
        CefKeyEvent key_event;
        key_event.type = type;
        key_event.modifiers = EventModifiers(event.state);
        key_event.native_key_code = static_cast<int>(event.keycode);
        key_event.windows_key_code = WindowsKeyCode(key, text, text_length);
        if (type == KEYEVENT_CHAR && text_length > 0) {
            key_event.character = static_cast<char16_t>(static_cast<unsigned char>(text[0]));
            key_event.unmodified_character = key_event.character;
        }
        browser_->GetHost()->SendKeyEvent(key_event);
    }

    std::uint32_t EventModifiers(unsigned int state) const {
        std::uint32_t modifiers = 0;
        if ((state & ShiftMask) != 0) {
            modifiers |= EVENTFLAG_SHIFT_DOWN;
        }
        if ((state & ControlMask) != 0) {
            modifiers |= EVENTFLAG_CONTROL_DOWN;
        }
        if ((state & Mod1Mask) != 0) {
            modifiers |= EVENTFLAG_ALT_DOWN;
        }
        return modifiers;
    }

    cef_mouse_button_type_t MouseButton(unsigned int button) const {
        if (button == Button2) {
            return MBT_MIDDLE;
        }
        if (button == Button3) {
            return MBT_RIGHT;
        }
        return MBT_LEFT;
    }

    int WindowsKeyCode(KeySym key, const char* text, int text_length) const {
        if (text_length == 1) {
            const unsigned char ch = static_cast<unsigned char>(text[0]);
            if (ch >= 'a' && ch <= 'z') {
                return ch - ('a' - 'A');
            }
            return ch;
        }
        switch (key) {
            case XK_BackSpace:
                return 0x08;
            case XK_Tab:
                return 0x09;
            case XK_Return:
            case XK_KP_Enter:
                return 0x0d;
            case XK_Escape:
                return 0x1b;
            case XK_Left:
                return 0x25;
            case XK_Up:
                return 0x26;
            case XK_Right:
                return 0x27;
            case XK_Down:
                return 0x28;
            case XK_Delete:
                return 0x2e;
            default:
                return static_cast<int>(key & 0xff);
        }
    }

    void HandleControlClick(ControlId control) {
        switch (control) {
            case ControlId::Back:
                if (browser_ && browser_->CanGoBack()) {
                    browser_->GoBack();
                }
                break;
            case ControlId::Forward:
                if (browser_ && browser_->CanGoForward()) {
                    browser_->GoForward();
                }
                break;
            case ControlId::Reload:
                if (browser_) {
                    browser_->Reload();
                }
                break;
            case ControlId::Stop:
                if (browser_) {
                    browser_->StopLoad();
                }
                break;
            case ControlId::Terminal:
                terminal_mode_enabled_ = !terminal_mode_enabled_;
                settings_.terminal_mode_enabled = terminal_mode_enabled_;
                SaveSettings();
                ApplyTerminalMode();
                status_text_ = terminal_mode_enabled_ ? "TERMINAL MODE ENABLED" : "TERMINAL MODE DISABLED";
                break;
            case ControlId::Scanlines:
                scanlines_enabled_ = !scanlines_enabled_;
                settings_.scanlines_enabled = scanlines_enabled_;
                SaveSettings();
                status_text_ = scanlines_enabled_ ? "SCANLINES ENABLED" : "SCANLINES DISABLED";
                break;
            case ControlId::Glow:
                glow_enabled_ = !glow_enabled_;
                settings_.glow_enabled = glow_enabled_;
                SaveSettings();
                status_text_ = glow_enabled_ ? "GLOW ENABLED" : "GLOW DISABLED";
                break;
            case ControlId::Flicker:
                flicker_intensity_ = (flicker_intensity_ + 1) % 3;
                settings_.flicker_intensity = flicker_intensity_;
                SaveSettings();
                status_text_ = "FLICKER " + std::to_string(flicker_intensity_);
                break;
            case ControlId::Settings:
                settings_panel_enabled_ = !settings_panel_enabled_;
                deck_space_enabled_ = false;
                status_text_ = settings_panel_enabled_ ? "DIAGNOSTICS OPEN" : "DIAGNOSTICS CLOSED";
                break;
            case ControlId::Deck:
                deck_space_enabled_ = !deck_space_enabled_;
                settings_panel_enabled_ = false;
                LoadDeckNodes();
                status_text_ = deck_space_enabled_ ? "DECK SPACE ACTIVE" : "BROWSER MODE ACTIVE";
                break;
            case ControlId::AddNode:
                AddNodeFromCurrentPage();
                break;
            case ControlId::Url:
                url_focused_ = true;
                if (browser_) {
                    browser_->GetHost()->SetFocus(false);
                }
                break;
            case ControlId::Go:
                NavigateFromUrlText();
                break;
            case ControlId::NewTab:
                status_text_ = "LINUX TAB STRIP: SINGLE ACTIVE TAB";
                break;
            case ControlId::Tab:
                settings_panel_enabled_ = false;
                deck_space_enabled_ = false;
                break;
        }
        Draw();
    }

    void NavigateFromUrlText() {
        if (url_text_.size() > kMaxUrlTextLength) {
            status_text_ = "NAVIGATION BLOCKED: ADDRESS TOO LONG";
            Draw();
            return;
        }

        const cyberdeck::browser::NormalizedNavigation normalized =
            cyberdeck::browser::NormalizeAddressBarInput(ToWide(url_text_));
        if (normalized.decision != cyberdeck::browser::NavigationDecision::kNavigate || normalized.target_url.empty()) {
            status_text_ = normalized.reason.empty() ? "NAVIGATION BLOCKED" : "NAVIGATION BLOCKED: " + ToUtf8(normalized.reason);
            Draw();
            return;
        }

        const std::string target = ToUtf8(normalized.target_url);
        url_text_ = target;
        current_url_ = normalized.target_url;
        url_focused_ = false;
        settings_panel_enabled_ = false;
        deck_space_enabled_ = false;
        status_text_ = "NAVIGATING";
        if (browser_) {
            browser_->GetMainFrame()->LoadURL(CefString(target));
            browser_->GetHost()->SetFocus(true);
        }
        Draw();
    }

    void ApplyTerminalMode() {
        if (!browser_) {
            return;
        }
        CefRefPtr<CefFrame> frame = browser_->GetMainFrame();
        if (!frame) {
            return;
        }
        frame->ExecuteJavaScript(
            CefString(terminal_mode_enabled_ ? TerminalModeInjectionScript() : TerminalModeRemovalScript()),
            CefString(std::wstring(L"cyberdeck://terminal-mode")),
            0);
    }

    void AddNodeFromCurrentPage() {
        if (current_url_.empty() || StartsWith(current_url_, L"about:")) {
            status_text_ = "ADD NODE NEEDS A WEBSITE";
            return;
        }

        const std::vector<cyberdeck::deck::BookmarkNode> current_nodes = bookmark_store_.LoadBookmarks();
        const std::string now = cyberdeck::deck::CurrentBookmarkNodeUtcTimestamp();
        int sequence = static_cast<int>(current_nodes.size()) + 1;
        std::wstring node_id;
        do {
            node_id = cyberdeck::deck::GenerateBookmarkNodeId(now, sequence++);
        } while (std::any_of(current_nodes.begin(), current_nodes.end(), [&node_id](const cyberdeck::deck::BookmarkNode& node) {
            return node.id == node_id;
        }));

        BrowserTabState tab;
        tab.id = 1;
        tab.title = current_title_.empty() ? current_url_ : current_title_;
        tab.url = current_url_;

        auto created = cyberdeck::deck::CreateBookmarkNodeFromActiveTab(tab, node_id, now);
        if (!created.success) {
            status_text_ = created.message.empty() ? "ADD NODE BLOCKED" : "ADD NODE BLOCKED: " + ToUtf8(created.message);
            return;
        }
        created.node.color_theme = cyberdeck::deck::BookmarkNodeColorTheme::Mixed;
        const auto favicon_path = cyberdeck::deck::FaviconStore::EnsurePlaceholderFavicon(
            cyberdeck::deck::FaviconStore::DefaultFaviconsDirectory(),
            created.node.url,
            created.node.title);
        if (favicon_path) {
            created.node.favicon_path = *favicon_path;
        }

        const auto duplicate = std::find_if(current_nodes.begin(), current_nodes.end(), [&created](const cyberdeck::deck::BookmarkNode& node) {
            return node.url == created.node.url;
        });
        if (duplicate != current_nodes.end()) {
            cyberdeck::deck::BookmarkNode updated = *duplicate;
            updated.title = created.node.title;
            updated.updated_utc = now;
            if (bookmark_store_.UpdateBookmark(std::move(updated))) {
                status_text_ = "NODE UPDATED IN DECK";
                LoadDeckNodes();
                return;
            }
            status_text_ = "NODE UPDATE FAILED";
            return;
        }

        if (bookmark_store_.AddBookmark(std::move(created.node))) {
            status_text_ = "NODE ADDED TO DECK";
            LoadDeckNodes();
        } else {
            status_text_ = "NODE ADD FAILED";
        }
    }

    void LoadDeckNodes() {
        deck_nodes_ = bookmark_store_.LoadBookmarks();
        if (selected_node_ >= deck_nodes_.size()) {
            selected_node_ = deck_nodes_.empty() ? 0 : deck_nodes_.size() - 1;
        }
    }

    void HandleDeckClick(int x, int y) {
        BuildDeckHits();
        std::vector<DeckHit> hit_order = deck_hits_;
        std::sort(hit_order.begin(), hit_order.end(), [](const DeckHit& left, const DeckHit& right) {
            return left.depth < right.depth;
        });
        for (const DeckHit& hit : hit_order) {
            if (Contains(hit.rect, x, y)) {
                selected_node_ = hit.index;
                OpenSelectedDeckNode();
                return;
            }
        }
    }

    void HandleDeckKey(KeySym key) {
        if (key == XK_Escape) {
            deck_space_enabled_ = false;
            status_text_ = "BROWSER MODE ACTIVE";
            Draw();
            return;
        }
        if (key == XK_Left && !deck_nodes_.empty()) {
            selected_node_ = selected_node_ == 0 ? deck_nodes_.size() - 1 : selected_node_ - 1;
        } else if (key == XK_Right && !deck_nodes_.empty()) {
            selected_node_ = (selected_node_ + 1) % deck_nodes_.size();
        } else if ((key == XK_Return || key == XK_KP_Enter) && !deck_nodes_.empty()) {
            OpenSelectedDeckNode();
        } else if (key == XK_l || key == XK_L) {
            deck_layout_mode_ = cyberdeck::render::NextLayoutMode(deck_layout_mode_);
            settings_.deck_layout_mode = ToWide(cyberdeck::render::ToLayoutModeString(deck_layout_mode_));
            SaveSettings();
            status_text_ = std::string("DECK LAYOUT: ") + cyberdeck::render::ToLayoutModeString(deck_layout_mode_);
        } else if (key == XK_Delete && !deck_nodes_.empty()) {
            const std::wstring deleted_id = deck_nodes_[selected_node_].id;
            if (bookmark_store_.DeleteBookmark(deleted_id)) {
                status_text_ = "NODE DELETED FROM DECK";
                LoadDeckNodes();
            } else {
                status_text_ = "NODE DELETE FAILED";
            }
        }
        Draw();
    }

    void OpenSelectedDeckNode() {
        if (deck_nodes_.empty() || selected_node_ >= deck_nodes_.size()) {
            return;
        }

        cyberdeck::deck::BookmarkNode node = deck_nodes_[selected_node_];
        const std::string now = cyberdeck::deck::CurrentBookmarkNodeUtcTimestamp();
        node.last_visited_utc = now;
        node.updated_utc = now;
        ++node.visit_count;
        bookmark_store_.UpdateBookmark(node);

        url_text_ = ToUtf8(node.url);
        current_url_ = node.url;
        deck_space_enabled_ = settings_.keep_deck_open_after_node_open;
        status_text_ = "NODE OPENED";
        if (browser_) {
            browser_->GetMainFrame()->LoadURL(CefString(url_text_));
        }
        Draw();
    }

    void SaveSettings() {
        if (settings_ready_) {
            settings_store_.Save(settings_);
        }
    }

    void ResizeBrowser() {
        if (browser_) {
            browser_->GetHost()->WasResized();
        }
    }

    void Close() {
        if (browser_) {
            browser_->GetHost()->CloseBrowser(false);
            browser_ = nullptr;
        } else {
            running_ = false;
        }
    }

    void Draw() {
        if (display_ == nullptr || window_ == 0 || gc_ == 0) {
            return;
        }

        Fill({0, 0, width_, height_}, kBlack);
        DrawToolbar();
        DrawTabStrip();

        if (deck_space_enabled_) {
            DrawDeck();
        } else if (settings_panel_enabled_) {
            DrawSettingsPanel();
        } else {
            DrawBrowserPixels();
        }

        if (scanlines_enabled_) {
            DrawScanlines({0, 0, width_, height_}, 4);
        }

        XFlush(display_);
    }

    void DrawToolbar() {
        Fill({0, 0, width_, kToolbarHeight}, kDarkPanel);
        Line(0, kToolbarHeight - 1, width_, kToolbarHeight - 1, kDimGreen);

        DrawButton(ControlId::Back, "BACK", can_go_back_, false);
        DrawButton(ControlId::Forward, "FWD", can_go_forward_, false);
        DrawButton(ControlId::Reload, "RELOAD", !loading_, false);
        DrawButton(ControlId::Stop, "STOP", loading_, false, kRed);
        DrawButton(ControlId::Terminal, "TERM", true, terminal_mode_enabled_, kYellow);
        DrawButton(ControlId::Scanlines, "SCAN", true, scanlines_enabled_, kYellow);
        DrawButton(ControlId::Glow, "GLOW", true, glow_enabled_, kYellow);
        DrawButton(ControlId::Flicker, "FLK" + std::to_string(flicker_intensity_), true, flicker_intensity_ > 0, kYellow);
        DrawButton(ControlId::Settings, "SET", true, settings_panel_enabled_, kYellow);
        DrawButton(ControlId::Deck, deck_space_enabled_ ? "WEB" : "DECK", true, deck_space_enabled_, kYellow);
        DrawButton(ControlId::AddNode, "ADD NODE", true, false, kYellow);
        DrawUrlEdit();
        DrawButton(ControlId::Go, "GO", true, false, kYellow);
    }

    void DrawTabStrip() {
        Fill({0, kToolbarHeight, width_, kTabStripHeight}, kDarkerPanel);
        const Rect tab = ControlBounds(ControlId::Tab);
        Fill(tab, 0x00160a);
        Frame(tab, glow_enabled_ ? kGreen : kDimGreen, glow_enabled_ ? 2 : 1);

        std::string title = ToUtf8(current_title_.empty() ? std::wstring(L"New Tab") : current_title_);
        if (loading_) {
            static constexpr const char* frames[] = {"[|] ", "[/] ", "[-] ", "[\\] "};
            title = std::string(frames[animation_frame_ % 4]) + title;
        }
        Text(tab.x + kTextInset, TextBaseline(tab), CompactForRect(title, tab, kTextInset * 2), kYellow);

        const Rect plus = ControlBounds(ControlId::NewTab);
        Fill(plus, 0x000e08);
        Frame(plus, kDimGreen);
        Text(plus.x + std::max(4, (plus.width - TextWidth("+")) / 2), TextBaseline(plus), "+", kGreen);

        const Rect status_rect{plus.x + plus.width + 16, kTabTop, std::max(0, width_ - plus.x - plus.width - 26), kTabHeight};
        const std::string status = CompactForRect(status_text_, status_rect);
        Text(status_rect.x, TextBaseline(status_rect), status, status_text_.find("FAILED") != std::string::npos ? kRed : kDimGreen);
    }

    void DrawButton(ControlId id, const std::string& label, bool enabled, bool active, unsigned long accent = kGreen) {
        const Rect rect = ControlBounds(id);
        if (rect.width <= 0) {
            return;
        }

        const unsigned long border = enabled ? (active ? kYellow : accent) : kFaintGreen;
        const unsigned long text = enabled ? (active ? kYellow : accent) : kDimGreen;
        Fill(rect, active ? 0x121200 : kBlack);
        if (active && glow_enabled_) {
            Frame({rect.x - 1, rect.y - 1, rect.width + 2, rect.height + 2}, border);
        }
        Frame(rect, border, active ? 2 : 1);
        if (scanlines_enabled_) {
            DrawScanlines(rect, 5);
        }
        const int text_x = rect.x + std::max(1, (rect.width - TextWidth(label)) / 2);
        Text(text_x, TextBaseline(rect), label, text);
    }

    void DrawUrlEdit() {
        const Rect rect = ControlBounds(ControlId::Url);
        Fill(rect, kBlack);
        Frame(rect, url_focused_ ? kYellow : kDimGreen, url_focused_ ? 2 : 1);
        const std::string visible = CompactForRect(url_text_, rect, kTextInset * 2);
        Text(rect.x + kTextInset, TextBaseline(rect), visible, url_focused_ ? kYellow : kGreen);
    }

    void DrawBrowserPixels() {
        const Rect web = WebRect();
        if (web_pixels_.empty() || web_width_ <= 0 || web_height_ <= 0) {
            Fill(web, kBlack);
            Frame({web.x + 28, web.y + 28, web.width - 56, 92}, kDimGreen);
            Text(web.x + 48, web.y + 64, "CEF SURFACE INITIALIZING", kGreen);
            Text(web.x + 48, web.y + 92, "URL " + Compact(ToUtf8(current_url_), 96), kYellow);
            return;
        }

        XImage* image = XCreateImage(
            display_,
            DefaultVisual(display_, screen_),
            static_cast<unsigned int>(DefaultDepth(display_, screen_)),
            ZPixmap,
            0,
            reinterpret_cast<char*>(web_pixels_.data()),
            static_cast<unsigned int>(web_width_),
            static_cast<unsigned int>(web_height_),
            32,
            0);
        if (image == nullptr) {
            return;
        }
        XPutImage(
            display_,
            window_,
            gc_,
            image,
            0,
            0,
            web.x,
            web.y,
            static_cast<unsigned int>(std::min(web.width, web_width_)),
            static_cast<unsigned int>(std::min(web.height, web_height_)));
        image->data = nullptr;
        XDestroyImage(image);
    }

    void DrawSettingsPanel() {
        const Rect web = WebRect();
        Fill(web, kBlack);
        const Rect panel{web.x + 34, web.y + 34, web.width - 68, web.height - 68};
        Fill(panel, kDarkPanel);
        Frame(panel, kGreen, glow_enabled_ ? 2 : 1);

        int y = panel.y + 34;
        Text(panel.x + 26, y, "CYBERDECK DIAGNOSTICS", kGreen);
        y += 34;
        Text(panel.x + 26, y, "version: " + ToUtf8(cyberdeck::common::AppVersion()), kYellow);
        y += 24;
        Text(panel.x + 26, y, "target: CyberDeckBrowserLinuxCef", kYellow);
        y += 24;
        Text(panel.x + 26, y, "render: CEF OSR X11", kYellow);
        y += 24;
        Text(panel.x + 26, y, "wsl: " + std::string(IsWsl() ? "true" : "false"), kYellow);
        y += 24;
        Text(panel.x + 26, y, "data: " + Compact(cyberdeck::common::AppDataDirectory().string(), 120), kDimGreen);
        y += 24;
        Text(panel.x + 26, y, "log: " + Compact(logger_.path().string(), 120), kDimGreen);
        y += 34;
        Text(panel.x + 26, y, "terminalModeEnabled: " + std::string(terminal_mode_enabled_ ? "true" : "false"), kGreen);
        y += 24;
        Text(panel.x + 26, y, "scanlinesEnabled: " + std::string(scanlines_enabled_ ? "true" : "false"), kGreen);
        y += 24;
        Text(panel.x + 26, y, "glowEnabled: " + std::string(glow_enabled_ ? "true" : "false"), kGreen);
        y += 24;
        Text(panel.x + 26, y, "flickerIntensity: " + std::to_string(flicker_intensity_), kGreen);
        y += 24;
        Text(panel.x + 26, y, "deckLayoutMode: " + std::string(cyberdeck::render::ToLayoutModeString(deck_layout_mode_)), kGreen);
        y += 34;
        Text(panel.x + 26, y, "bookmarks: " + std::to_string(bookmark_store_.LoadBookmarks().size()), kYellow);
        y += 24;
        Text(panel.x + 26, y, "current: " + Compact(ToUtf8(current_url_), 112), kYellow);
    }

    void DrawDeck() {
        const Rect web = WebRect();
        Fill(web, kBlack);
        LoadDeckNodes();
        BuildDeckHits();

        DrawDeckGrid3D(web);

        Text(web.x + 30, web.y + 38, "DECK SPACE 3D", kGreen);
        Text(web.x + 30, web.y + 64, std::string(cyberdeck::render::ToLayoutModeString(deck_layout_mode_)), kYellow);

        std::vector<DeckHit> draw_order = deck_hits_;
        std::sort(draw_order.begin(), draw_order.end(), [](const DeckHit& left, const DeckHit& right) {
            return left.depth > right.depth;
        });
        for (const DeckHit& hit : draw_order) {
            DrawDeckNode3D(hit.index);
        }

        if (deck_nodes_.empty()) {
            Rect empty{web.x + 60, web.y + 110, web.width - 120, 90};
            Frame(empty, kDimGreen);
            Text(empty.x + 24, empty.y + 38, "NO DECK NODES FOUND", kYellow);
        }
    }

    void BuildDeckHits() {
        deck_hits_.clear();
        const Rect web = WebRect();
        if (deck_nodes_.empty()) {
            return;
        }

        const std::vector<cyberdeck::render::DeckLayoutItem> layout =
            cyberdeck::render::BuildDeckLayout(deck_layout_mode_, deck_nodes_.size());

        for (std::size_t index = 0; index < deck_nodes_.size(); ++index) {
            const cyberdeck::deck::BookmarkNode& node = deck_nodes_[index];
            const cyberdeck::render::DeckLayoutItem& item = LayoutItemForNode(layout, index);
            const cyberdeck::render::DeckMesh mesh = MeshForShape(ShapeFromNode(node, deck_layout_mode_));

            int min_x = width_;
            int min_y = height_;
            int max_x = 0;
            int max_y = 0;
            float depth_sum = 0.0f;
            int point_count = 0;
            for (const cyberdeck::render::Vec3& vertex : mesh.vertices) {
                const ProjectedPoint projected = ProjectDeckPoint(TransformDeckVertex(vertex, node, item, index), web);
                min_x = std::min(min_x, projected.x);
                min_y = std::min(min_y, projected.y);
                max_x = std::max(max_x, projected.x);
                max_y = std::max(max_y, projected.y);
                depth_sum += projected.depth;
                ++point_count;
            }

            const bool selected = index == selected_node_;
            const int padding = selected ? 22 : 14;
            if (point_count > 0) {
                deck_hits_.push_back({
                    index,
                    {min_x - padding, min_y - padding, max_x - min_x + padding * 2, max_y - min_y + padding * 2},
                    depth_sum / static_cast<float>(point_count),
                });
            }
        }
    }

    const cyberdeck::render::DeckLayoutItem& LayoutItemForNode(
        const std::vector<cyberdeck::render::DeckLayoutItem>& layout,
        std::size_t index) const {
        const std::size_t layout_index = deck_nodes_.empty()
                                             ? 0
                                             : (index + deck_nodes_.size() - selected_node_) % deck_nodes_.size();
        return layout[layout_index % layout.size()];
    }

    cyberdeck::render::Vec3 DeckPositionForNode(
        const cyberdeck::deck::BookmarkNode& node,
        const cyberdeck::render::DeckLayoutItem& item) const {
        if (node.deck_position) {
            return {node.deck_position->x, node.deck_position->y, node.deck_position->z};
        }
        return item.position;
    }

    cyberdeck::render::Vec3 DeckScaleForNode(
        const cyberdeck::render::DeckLayoutItem& item,
        bool selected,
        std::size_t index) const {
        cyberdeck::render::Vec3 scale = item.scale;
        if (!selected) {
            scale = {scale.x * 0.88f, scale.y * 0.88f, scale.z * 0.88f};
        }
        const float phase = static_cast<float>(animation_frame_) * 0.62f + static_cast<float>(index) * 0.37f;
        const float pulse = selected ? 1.0f + std::sin(phase) * 0.045f : 1.0f + std::sin(phase) * 0.012f;
        return {scale.x * pulse, scale.y * pulse, scale.z * pulse};
    }

    cyberdeck::render::Vec3 TransformDeckVertex(
        cyberdeck::render::Vec3 vertex,
        const cyberdeck::deck::BookmarkNode& node,
        const cyberdeck::render::DeckLayoutItem& item,
        std::size_t index) const {
        const bool selected = index == selected_node_;
        const cyberdeck::render::Vec3 scale = DeckScaleForNode(item, selected, index);
        vertex = {vertex.x * scale.x, vertex.y * scale.y, vertex.z * scale.z};
        vertex = RotateX(vertex, item.rotation_degrees.x + (selected ? -3.0f : 0.0f));
        vertex = RotateY(
            vertex,
            item.rotation_degrees.y + static_cast<float>(animation_frame_) * (selected ? 4.8f : 2.2f) +
                static_cast<float>((index * 19) % 17));
        vertex = RotateZ(vertex, item.rotation_degrees.z);

        const cyberdeck::render::Vec3 position = DeckPositionForNode(node, item);
        return {vertex.x + position.x, vertex.y + position.y, vertex.z + position.z};
    }

    ProjectedPoint ProjectDeckPoint(const cyberdeck::render::Vec3& point, const Rect& web) const {
        constexpr float kCameraZ = 13.0f;
        constexpr float kCameraY = 0.95f;
        const float depth = std::max(1.0f, kCameraZ - point.z);
        const float focal = static_cast<float>(std::min(web.width, web.height)) * 0.92f;
        const float perspective = focal / depth;
        const int center_x = web.x + web.width / 2;
        const int horizon_y = web.y + static_cast<int>(static_cast<float>(web.height) * 0.36f);
        return {
            center_x + static_cast<int>(std::lround(point.x * perspective)),
            horizon_y + static_cast<int>(std::lround((kCameraY - point.y) * perspective)),
            depth,
        };
    }

    void DrawDeckGrid3D(const Rect& web) {
        const cyberdeck::render::Vec3 left_front{-9.0f, -1.35f, 5.8f};
        const cyberdeck::render::Vec3 right_front{9.0f, -1.35f, 5.8f};
        const cyberdeck::render::Vec3 left_back{-9.0f, -1.35f, -8.0f};
        const cyberdeck::render::Vec3 right_back{9.0f, -1.35f, -8.0f};
        const ProjectedPoint lf = ProjectDeckPoint(left_front, web);
        const ProjectedPoint rf = ProjectDeckPoint(right_front, web);
        const ProjectedPoint lb = ProjectDeckPoint(left_back, web);
        const ProjectedPoint rb = ProjectDeckPoint(right_back, web);

        Line(lb.x, lb.y, lf.x, lf.y, kFaintGreen);
        Line(rb.x, rb.y, rf.x, rf.y, kFaintGreen);
        Line(lf.x, lf.y, rf.x, rf.y, kDimGreen);
        Line(lb.x, lb.y, rb.x, rb.y, kFaintGreen);

        for (int line = -8; line <= 8; ++line) {
            const unsigned long color = line == 0 ? kDimGreen : kFaintGreen;
            const ProjectedPoint near = ProjectDeckPoint({static_cast<float>(line), -1.35f, 5.8f}, web);
            const ProjectedPoint far = ProjectDeckPoint({static_cast<float>(line), -1.35f, -8.0f}, web);
            Line(near.x, near.y, far.x, far.y, color);
        }
        for (int line = -8; line <= 6; line += 2) {
            const unsigned long color = line == 0 ? kDimGreen : kFaintGreen;
            const ProjectedPoint left = ProjectDeckPoint({-9.0f, -1.35f, static_cast<float>(line)}, web);
            const ProjectedPoint right = ProjectDeckPoint({9.0f, -1.35f, static_cast<float>(line)}, web);
            Line(left.x, left.y, right.x, right.y, color);
        }
    }

    void DrawDeckNode3D(std::size_t index) {
        if (index >= deck_nodes_.size()) {
            return;
        }

        const Rect web = WebRect();
        const std::vector<cyberdeck::render::DeckLayoutItem> layout =
            cyberdeck::render::BuildDeckLayout(deck_layout_mode_, deck_nodes_.size());
        const cyberdeck::deck::BookmarkNode& node = deck_nodes_[index];
        const cyberdeck::render::DeckLayoutItem& item = LayoutItemForNode(layout, index);
        const cyberdeck::render::DeckMesh mesh = MeshForShape(ShapeFromNode(node, deck_layout_mode_));
        const bool selected = index == selected_node_;
        const unsigned long color = NodeColor(node, index);

        std::vector<cyberdeck::render::Vec3> transformed;
        transformed.reserve(mesh.vertices.size());
        for (const cyberdeck::render::Vec3& vertex : mesh.vertices) {
            transformed.push_back(TransformDeckVertex(vertex, node, item, index));
        }

        std::vector<ProjectedFace> faces;
        faces.reserve(mesh.faces.size());
        for (const cyberdeck::render::MeshFace& face : mesh.faces) {
            ProjectedFace projected_face;
            projected_face.selected = selected;
            float depth_sum = 0.0f;
            for (int vertex_index : face.indices) {
                if (vertex_index < 0 || vertex_index >= static_cast<int>(transformed.size())) {
                    continue;
                }
                const ProjectedPoint projected =
                    ProjectDeckPoint(transformed[static_cast<std::size_t>(vertex_index)], web);
                projected_face.points.push_back({static_cast<short>(projected.x), static_cast<short>(projected.y)});
                depth_sum += projected.depth;
            }
            if (projected_face.points.size() < 3) {
                continue;
            }

            projected_face.depth = depth_sum / static_cast<float>(projected_face.points.size());
            const float near_factor = std::clamp((14.0f - projected_face.depth) / 9.0f, 0.12f, 1.0f);
            projected_face.fill = selected ? ScaleColor(kYellow, 0.16f + near_factor * 0.12f)
                                           : ScaleColor(color, 0.10f + near_factor * 0.16f);
            projected_face.line = selected ? kYellow : ScaleColor(color, 0.58f + near_factor * 0.58f);
            faces.push_back(std::move(projected_face));
        }

        std::sort(faces.begin(), faces.end(), [](const ProjectedFace& left, const ProjectedFace& right) {
            return left.depth > right.depth;
        });

        for (const ProjectedFace& face : faces) {
            std::vector<XPoint> points = face.points;
            SetColor(face.fill);
            XFillPolygon(
                display_,
                window_,
                gc_,
                points.data(),
                static_cast<int>(points.size()),
                Convex,
                CoordModeOrigin);
            SetColor(face.line);
            XSetLineAttributes(display_, gc_, face.selected ? 2 : 1, LineSolid, CapButt, JoinMiter);
            points.push_back(points.front());
            XDrawLines(display_, window_, gc_, points.data(), static_cast<int>(points.size()), CoordModeOrigin);
            XSetLineAttributes(display_, gc_, 1, LineSolid, CapButt, JoinMiter);
        }

        auto hit = std::find_if(deck_hits_.begin(), deck_hits_.end(), [index](const DeckHit& candidate) {
            return candidate.index == index;
        });
        if (hit != deck_hits_.end()) {
            if (selected && glow_enabled_) {
                Frame({hit->rect.x - 5, hit->rect.y - 5, hit->rect.width + 10, hit->rect.height + 10}, kYellow, 2);
            }
            const std::string label = CompactForRect(ToUtf8(node.title.empty() ? node.url : node.title), hit->rect, 8);
            Text(hit->rect.x, hit->rect.y + hit->rect.height + 24, label, selected ? kYellow : color);
        }
    }

    unsigned long NodeColor(const cyberdeck::deck::BookmarkNode& node, std::size_t index) const {
        switch (node.color_theme) {
            case cyberdeck::deck::BookmarkNodeColorTheme::Green:
                return kGreen;
            case cyberdeck::deck::BookmarkNodeColorTheme::Yellow:
                return kYellow;
            case cyberdeck::deck::BookmarkNodeColorTheme::Red:
                return kRed;
            case cyberdeck::deck::BookmarkNodeColorTheme::Mixed:
                return index % 3 == 0 ? kGreen : (index % 3 == 1 ? kYellow : kRed);
        }
        return kGreen;
    }

    void DrawNode(const Rect& rect, cyberdeck::deck::BookmarkNodeShapeType shape, unsigned long color, bool selected) {
        if (shape == cyberdeck::deck::BookmarkNodeShapeType::Hex) {
            XPoint points[6] = {
                {static_cast<short>(rect.x + rect.width / 2), static_cast<short>(rect.y)},
                {static_cast<short>(rect.x + rect.width), static_cast<short>(rect.y + rect.height / 4)},
                {static_cast<short>(rect.x + rect.width), static_cast<short>(rect.y + rect.height * 3 / 4)},
                {static_cast<short>(rect.x + rect.width / 2), static_cast<short>(rect.y + rect.height)},
                {static_cast<short>(rect.x), static_cast<short>(rect.y + rect.height * 3 / 4)},
                {static_cast<short>(rect.x), static_cast<short>(rect.y + rect.height / 4)},
            };
            SetColor(selected ? 0x1a1a00 : 0x001006);
            XFillPolygon(display_, window_, gc_, points, 6, Convex, CoordModeOrigin);
            SetColor(selected ? kYellow : color);
            XDrawLines(display_, window_, gc_, points, 6, CoordModeOrigin);
            XDrawLine(display_, window_, gc_, points[5].x, points[5].y, points[0].x, points[0].y);
        } else if (shape == cyberdeck::deck::BookmarkNodeShapeType::Cube) {
            Fill(rect, selected ? 0x1a1a00 : 0x001006);
            Frame(rect, selected ? kYellow : color, selected ? 2 : 1);
            Line(rect.x, rect.y, rect.x + 14, rect.y - 14, color);
            Line(rect.x + rect.width, rect.y, rect.x + rect.width + 14, rect.y - 14, color);
            Line(rect.x + 14, rect.y - 14, rect.x + rect.width + 14, rect.y - 14, color);
            Line(rect.x + rect.width + 14, rect.y - 14, rect.x + rect.width + 14, rect.y + rect.height - 14, color);
            Line(rect.x + rect.width, rect.y + rect.height, rect.x + rect.width + 14, rect.y + rect.height - 14, color);
        } else {
            Fill(rect, selected ? 0x1a1a00 : 0x001006);
            Frame(rect, selected ? kYellow : color, selected ? 2 : 1);
            Frame({rect.x + 7, rect.y + 7, rect.width - 14, rect.height - 14}, color);
        }
        if (glow_enabled_ && selected) {
            Frame({rect.x - 4, rect.y - 4, rect.width + 8, rect.height + 8}, kYellow);
        }
    }

    void Fill(const Rect& rect, unsigned long color) {
        SetColor(color);
        XFillRectangle(
            display_,
            window_,
            gc_,
            rect.x,
            rect.y,
            static_cast<unsigned int>(std::max(0, rect.width)),
            static_cast<unsigned int>(std::max(0, rect.height)));
    }

    void Frame(const Rect& rect, unsigned long color, int width = 1) {
        SetColor(color);
        for (int offset = 0; offset < width; ++offset) {
            XDrawRectangle(
                display_,
                window_,
                gc_,
                rect.x + offset,
                rect.y + offset,
                static_cast<unsigned int>(std::max(0, rect.width - 1 - offset * 2)),
                static_cast<unsigned int>(std::max(0, rect.height - 1 - offset * 2)));
        }
    }

    void Line(int x1, int y1, int x2, int y2, unsigned long color) {
        SetColor(color);
        XDrawLine(display_, window_, gc_, x1, y1, x2, y2);
    }

    void Text(int x, int y, const std::string& text, unsigned long color) {
        SetColor(color);
        int cursor_x = x;
        const int top = y - kTextPixelHeight;
        for (char ch : text) {
            const GlyphRows glyph = GlyphFor(ch);
            for (int row = 0; row < kGlyphRows; ++row) {
                const std::uint8_t bits = glyph[static_cast<std::size_t>(row)];
                for (int col = 0; col < kGlyphColumns; ++col) {
                    const int mask = 1 << (kGlyphColumns - 1 - col);
                    if ((bits & mask) == 0) {
                        continue;
                    }
                    XFillRectangle(
                        display_,
                        window_,
                        gc_,
                        cursor_x + col * kGlyphScaleX,
                        top + row * kGlyphScaleY,
                        static_cast<unsigned int>(kGlyphScaleX),
                        static_cast<unsigned int>(kGlyphScaleY));
                }
            }
            cursor_x += kApproxCharWidth;
        }
    }

    void DrawScanlines(const Rect& rect, int spacing) {
        SetColor(0x001200);
        for (int y = rect.y + spacing; y < rect.y + rect.height; y += spacing) {
            XDrawLine(display_, window_, gc_, rect.x, y, rect.x + rect.width, y);
        }
        if (flicker_intensity_ > 0 && ((animation_frame_ / 3) % 8) == 0) {
            SetColor(flicker_intensity_ == 1 ? kFaintGreen : kDimGreen);
            const int y = rect.y + 8 + static_cast<int>((animation_frame_ * 17) % std::max(1, rect.height - 16));
            XDrawLine(display_, window_, gc_, rect.x, y, rect.x + rect.width, y);
        }
    }

    void SetColor(unsigned long color) {
        XSetForeground(display_, gc_, color);
    }

    void ShutdownX11() {
        if (display_ == nullptr) {
            return;
        }
        if (gc_ != 0) {
            XFreeGC(display_, gc_);
            gc_ = 0;
        }
        if (window_ != 0) {
            XDestroyWindow(display_, window_);
            window_ = 0;
        }
        XCloseDisplay(display_);
        display_ = nullptr;
    }

    std::string startup_url_;
    bool smoke_test_ = false;
    bool start_deck_ = false;
    bool running_ = true;
    bool browser_created_ = false;
    bool logging_ready_ = false;
    bool settings_ready_ = false;

    cyberdeck::common::Logger logger_;
    cyberdeck::settings::SettingsStore settings_store_;
    cyberdeck::settings::UserSettings settings_;
    cyberdeck::history::HistoryStore history_store_;
    cyberdeck::deck::BookmarkStore bookmark_store_;

    Display* display_ = nullptr;
    int screen_ = 0;
    Window window_ = 0;
    GC gc_ = 0;
    Atom wm_delete_ = 0;
    int width_ = kInitialWidth;
    int height_ = kInitialHeight;

    CefRefPtr<CefBrowser> browser_;
    CefRefPtr<LinuxBrowserClient> client_;

    std::vector<std::uint32_t> web_pixels_;
    int web_width_ = 0;
    int web_height_ = 0;
    int paint_count_ = 0;

    std::wstring current_title_ = L"New Tab";
    std::wstring current_url_;
    std::string url_text_;
    std::string status_text_;
    bool loading_ = false;
    bool can_go_back_ = false;
    bool can_go_forward_ = false;
    bool url_focused_ = false;
    bool terminal_mode_enabled_ = false;
    bool scanlines_enabled_ = true;
    bool glow_enabled_ = true;
    int flicker_intensity_ = 0;
    int animation_frame_ = 0;
    bool deck_space_enabled_ = false;
    bool settings_panel_enabled_ = false;

    cyberdeck::render::DeckLayoutMode deck_layout_mode_ = cyberdeck::render::DeckLayoutMode::HexRing;
    std::vector<cyberdeck::deck::BookmarkNode> deck_nodes_;
    std::vector<DeckHit> deck_hits_;
    std::size_t selected_node_ = 0;
};

void LinuxBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    shell_.SetBrowser(browser);
}

void LinuxBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    shell_.BrowserClosed(browser);
}

void LinuxBrowserClient::OnTitleChange(CefRefPtr<CefBrowser>, const CefString& title) {
    shell_.UpdateTitle(title.ToWString());
}

void LinuxBrowserClient::OnAddressChange(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, const CefString& url) {
    if (frame && frame->IsMain()) {
        shell_.UpdateAddress(url.ToWString());
    }
}

void LinuxBrowserClient::OnLoadingStateChange(
    CefRefPtr<CefBrowser>,
    bool is_loading,
    bool can_go_back,
    bool can_go_forward) {
    shell_.UpdateLoading(is_loading, can_go_back, can_go_forward);
}

void LinuxBrowserClient::OnLoadStart(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, TransitionType) {
    if (frame && frame->IsMain()) {
        shell_.LoadStarted(frame->GetURL().ToWString());
    }
}

void LinuxBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, int http_status_code) {
    if (frame && frame->IsMain()) {
        shell_.LoadEnded(http_status_code);
    }
}

void LinuxBrowserClient::OnLoadError(
    CefRefPtr<CefBrowser>,
    CefRefPtr<CefFrame> frame,
    ErrorCode error_code,
    const CefString&,
    const CefString& failed_url) {
    if (frame && frame->IsMain()) {
        shell_.LoadFailed(static_cast<int>(error_code), failed_url.ToWString());
    }
}

bool LinuxBrowserClient::OnBeforeBrowse(
    CefRefPtr<CefBrowser>,
    CefRefPtr<CefFrame>,
    CefRefPtr<CefRequest> request,
    bool user_gesture,
    bool) {
    if (!request) {
        return false;
    }
    return shell_.HandleProtocolNavigation(request->GetURL().ToWString(), "OnBeforeBrowse", user_gesture);
}

bool LinuxBrowserClient::OnOpenURLFromTab(
    CefRefPtr<CefBrowser>,
    CefRefPtr<CefFrame>,
    const CefString& target_url,
    cef_window_open_disposition_t,
    bool user_gesture) {
    return shell_.HandleProtocolNavigation(target_url.ToWString(), "OnOpenURLFromTab", user_gesture);
}

void LinuxBrowserClient::GetViewRect(CefRefPtr<CefBrowser>, CefRect& rect) {
    shell_.ViewRect(rect);
}

bool LinuxBrowserClient::GetScreenPoint(CefRefPtr<CefBrowser>, int view_x, int view_y, int& screen_x, int& screen_y) {
    return shell_.ScreenPoint(view_x, view_y, screen_x, screen_y);
}

void LinuxBrowserClient::OnPaint(
    CefRefPtr<CefBrowser>,
    PaintElementType type,
    const RectList&,
    const void* buffer,
    int width,
    int height) {
    if (type == PET_VIEW) {
        shell_.BrowserPaint(buffer, width, height);
    }
}

void ConfigureSettings(CefSettings& settings) {
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;
    settings.persist_session_cookies = true;

    const std::filesystem::path data_dir = cyberdeck::common::AppDataDirectory();
    const std::filesystem::path cef_dir = data_dir / "cef";
    const std::filesystem::path cache_dir = cef_dir / "cache";
    const std::filesystem::path log_dir = data_dir / "logs";

    std::error_code error;
    std::filesystem::create_directories(cache_dir, error);
    std::filesystem::create_directories(log_dir, error);

    CefString(&settings.root_cache_path).FromString(cef_dir.string());
    CefString(&settings.cache_path).FromString(cache_dir.string());
    CefString(&settings.log_file).FromString((log_dir / "cef.log").string());
}

}  // namespace

int main(int argc, char** argv) {
    const bool smoke_test = HasSwitch(argc, argv, "--cyberdeck-smoke-test");
    const bool enable_gpu = HasSwitch(argc, argv, "--cyberdeck-enable-gpu");
    const bool start_deck = HasSwitch(argc, argv, "--cyberdeck-start-deck");

    CefMainArgs main_args(argc, argv);
    CefRefPtr<LinuxBrowserApp> app = new LinuxBrowserApp(enable_gpu);

    const int process_exit_code = CefExecuteProcess(main_args, app, nullptr);
    if (process_exit_code >= 0) {
        return process_exit_code;
    }

    CefSettings settings;
    ConfigureSettings(settings);
    if (!CefInitialize(main_args, settings, app, nullptr)) {
        std::cerr << "Failed to initialize CEF.\n";
        return EXIT_FAILURE;
    }

    std::cout << ToUtf8(cyberdeck::common::AppName()) << " Linux CEF browser\n";
    std::cout << "version: " << ToUtf8(cyberdeck::common::AppVersion()) << '\n';
    std::cout << "data: " << cyberdeck::common::AppDataDirectory().string() << '\n';
    std::cout << "log: " << cyberdeck::common::Logger::DefaultLogPath().string() << '\n';
    std::cout << "render: CEF OSR X11\n";

    LinuxCyberDeckShell shell(StartupUrl(argc, argv), smoke_test, start_deck);
    const int exit_code = shell.Initialize() ? shell.Run() : EXIT_FAILURE;

    CefShutdown();
    return exit_code;
}
