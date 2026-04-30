#include "deck/FaviconStore.h"

#include "common/Platform.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

std::filesystem::path TestRoot() {
    return std::filesystem::temp_directory_path() /
           ("CyberDeckFaviconStoreTests-" + std::to_string(cyberdeck::common::CurrentProcessId()));
}

bool Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

std::string ReadText(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

}  // namespace

int main() {
    const std::filesystem::path root = TestRoot();
    std::error_code error;
    std::filesystem::remove_all(root, error);

    const std::filesystem::path favicons = root / "CyberDeckBrowser" / "favicons";
    bool passed = true;

    const auto path = cyberdeck::deck::FaviconStore::EnsurePlaceholderFavicon(
        favicons,
        L"https://www.example.com/articles",
        L"Example Domain");
    passed = Expect(path.has_value(), "Placeholder favicon should be created.") && passed;
    if (path) {
        passed = Expect(std::filesystem::exists(*path), "Placeholder favicon file should exist.") && passed;
        passed = Expect(std::filesystem::path(*path).parent_path() == favicons, "Favicon should live in favicons directory.") &&
                 passed;
        const std::string text = ReadText(*path);
        passed = Expect(text.find("<svg") != std::string::npos, "Placeholder favicon should be SVG.") && passed;
        passed = Expect(text.find("E</text>") != std::string::npos, "Placeholder should include title initial.") && passed;
    }

    const auto repeated = cyberdeck::deck::FaviconStore::EnsurePlaceholderFavicon(
        favicons,
        L"https://www.example.com/other",
        L"Changed Title");
    passed = Expect(repeated == path, "Same host should reuse deterministic favicon path.") && passed;

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
