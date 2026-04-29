#include "deck/BookmarkNode.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

bool Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

}  // namespace

int main() {
    bool passed = true;

    const std::string now = "2026-04-29T03:45:00Z";
    const std::wstring generated_id = cyberdeck::deck::GenerateBookmarkNodeId(now, 7);
    passed = Expect(generated_id == L"node-20260429t034500z-000007", "Node id should be stable and filesystem-safe.") &&
             passed;

    cyberdeck::browser::BrowserTabState tab;
    tab.title = L"Example Domain";
    tab.url = L"example.com";
    const auto created = cyberdeck::deck::CreateBookmarkNodeFromActiveTab(tab, L"node-example", now);
    passed = Expect(created.success, "Active tab should create a valid bookmark node.") && passed;
    passed = Expect(created.node.title == L"Example Domain", "Node title should come from the active tab.") && passed;
    passed = Expect(created.node.url == L"https://example.com", "Node URL should be normalized.") && passed;
    passed = Expect(
                 created.node.shape_type == cyberdeck::deck::BookmarkNodeShapeType::Hex,
                 "Default Node shape should be hex.") &&
             passed;
    passed = Expect(
                 created.node.color_theme == cyberdeck::deck::BookmarkNodeColorTheme::Green,
                 "Default Node color should be green.") &&
             passed;

    cyberdeck::deck::BookmarkNode rich = created.node;
    rich.favicon_path = L"favicons\\example.ico";
    rich.shape_type = cyberdeck::deck::BookmarkNodeShapeType::Panel;
    rich.color_theme = cyberdeck::deck::BookmarkNodeColorTheme::Mixed;
    rich.last_visited_utc = now;
    rich.visit_count = 3;
    rich.deck_position = cyberdeck::deck::BookmarkNodePosition{.x = 1.25f, .y = -0.5f, .z = 2.0f};
    rich.tags = {L"docs", L"daily"};

    const std::string json = cyberdeck::deck::BookmarkNodeToJson(rich);
    passed = Expect(json.find("\"shapeType\": \"panel\"") != std::string::npos, "JSON should use stable shape strings.") &&
             passed;
    passed = Expect(json.find("\"colorTheme\": \"mixed\"") != std::string::npos, "JSON should use stable color strings.") &&
             passed;
    passed = Expect(json.find("\"deckPosition\"") != std::string::npos, "JSON should include deck position field.") &&
             passed;

    std::wstring parse_error;
    const auto parsed = cyberdeck::deck::BookmarkNodeFromJson(json, &parse_error);
    passed = Expect(parsed.has_value(), "Serialized Node JSON should parse.") && passed;
    if (parsed) {
        passed = Expect(parsed->id == rich.id, "Parsed id should round-trip.") && passed;
        passed = Expect(parsed->url == rich.url, "Parsed URL should round-trip.") && passed;
        passed = Expect(parsed->favicon_path == rich.favicon_path, "Parsed favicon should round-trip.") && passed;
        passed = Expect(parsed->deck_position.has_value(), "Parsed deck position should be present.") && passed;
        passed = Expect(parsed->tags.size() == 2 && parsed->tags[1] == L"daily", "Parsed tags should round-trip.") &&
                 passed;
    }

    cyberdeck::deck::BookmarkNode invalid_title = created.node;
    invalid_title.title = L"   ";
    passed = Expect(
                 !cyberdeck::deck::ValidateBookmarkNode(invalid_title).valid,
                 "Validation should reject empty titles.") &&
             passed;

    cyberdeck::deck::BookmarkNode unsafe = created.node;
    unsafe.url = L"file:///C:/Windows/win.ini";
    passed = Expect(!cyberdeck::deck::ValidateBookmarkNode(unsafe).valid, "Validation should reject unsafe URLs.") &&
             passed;

    cyberdeck::deck::BookmarkNode unnormalized = created.node;
    unnormalized.url = L"example.org";
    passed = Expect(
                 !cyberdeck::deck::ValidateBookmarkNode(unnormalized).valid,
                 "Validation should reject URLs that have not been normalized.") &&
             passed;

    const std::string invalid_shape_json =
        "{"
        "\"id\":\"node-bad\","
        "\"title\":\"Bad\","
        "\"url\":\"https://example.com\","
        "\"shapeType\":\"sphere\","
        "\"colorTheme\":\"green\","
        "\"createdUtc\":\"2026-04-29T03:45:00Z\","
        "\"updatedUtc\":\"2026-04-29T03:45:00Z\","
        "\"visitCount\":0"
        "}";
    passed = Expect(
                 !cyberdeck::deck::BookmarkNodeFromJson(invalid_shape_json).has_value(),
                 "Parser should reject invalid shape strings.") &&
             passed;

    cyberdeck::browser::BrowserTabState blocked_tab;
    blocked_tab.title = L"Local file";
    blocked_tab.url = L"file:///C:/Windows/win.ini";
    passed = Expect(
                 !cyberdeck::deck::CreateBookmarkNodeFromActiveTab(blocked_tab, L"node-file", now).success,
                 "Active tab helper should reject unsafe URLs.") &&
             passed;

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
