#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

namespace cyberdeck::common {

class Logger {
public:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static std::filesystem::path DefaultLogPath();

    bool Initialize(const std::filesystem::path& log_path);
    void Info(std::string_view message);
    void Error(std::string_view message);
    std::filesystem::path path() const;

private:
    bool RotateIfNeeded(const std::filesystem::path& log_path) const;
    void Write(std::string_view level, std::string_view message);

    std::ofstream stream_;
    std::filesystem::path log_path_;
};

}  // namespace cyberdeck::common
