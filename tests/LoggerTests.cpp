#include "common/Logger.h"

#include "common/Platform.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::filesystem::path TestRoot() {
    return std::filesystem::temp_directory_path() /
           ("CyberDeckLoggerTests-" + std::to_string(cyberdeck::common::CurrentProcessId()));
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

    const std::filesystem::path log_path = root / "CyberDeckBrowser" / "logs" / "cyberdeck.log";
    bool passed = true;

    {
        cyberdeck::common::Logger logger;
        passed = Expect(logger.Initialize(log_path), "Logger should initialize and create directories.") && passed;
        passed = Expect(logger.path() == log_path, "Logger should expose its active path.") && passed;
        logger.Info("startup marker");
        logger.Error("error marker");
    }

    const std::string first_text = ReadText(log_path);
    passed = Expect(first_text.find("startup marker") != std::string::npos, "Logger should write info lines.") && passed;
    passed = Expect(first_text.find("error marker") != std::string::npos, "Logger should write error lines.") && passed;

    {
        std::ofstream output(log_path, std::ios::binary | std::ios::trunc);
        std::string chunk(1024, 'x');
        for (int index = 0; index < 1030; ++index) {
            output << chunk;
        }
    }

    {
        cyberdeck::common::Logger logger;
        passed = Expect(logger.Initialize(log_path), "Logger should initialize after rotating a large file.") && passed;
        logger.Info("after rotation");
    }

    int rotated_count = 0;
    for (const auto& item : std::filesystem::directory_iterator(log_path.parent_path())) {
        const std::wstring name = item.path().filename().wstring();
        if (name.find(L"cyberdeck.") == 0 && item.path().extension() == L".log") {
            ++rotated_count;
        }
    }
    passed = Expect(rotated_count >= 1, "Oversized logs should be rotated.") && passed;
    passed = Expect(ReadText(log_path).find("after rotation") != std::string::npos, "New log should receive fresh writes.") &&
             passed;

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
