#include "common/Logger.h"

#include <windows.h>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <system_error>

namespace cyberdeck::common {
namespace {

std::string CurrentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};
#if defined(_MSC_VER)
    localtime_s(&local_time, &time);
#elif defined(__MINGW32__)
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    std::ostringstream output;
    output << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
    return output.str();
}

std::string CurrentFileTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};
#if defined(_MSC_VER)
    localtime_s(&local_time, &time);
#elif defined(__MINGW32__)
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    std::ostringstream output;
    output << std::put_time(&local_time, "%Y%m%d-%H%M%S");
    return output.str();
}

std::wstring Utf8ToWide(std::string_view value) {
    return std::wstring(value.begin(), value.end());
}

}  // namespace

std::filesystem::path Logger::DefaultLogPath() {
    DWORD required = GetEnvironmentVariableW(L"APPDATA", nullptr, 0);
    std::filesystem::path root;
    if (required > 0) {
        std::wstring app_data(required, L'\0');
        const DWORD copied = GetEnvironmentVariableW(L"APPDATA", app_data.data(), required);
        if (copied > 0) {
            app_data.resize(copied);
            root = app_data;
        }
    }

    if (root.empty()) {
        root = std::filesystem::current_path() / "dev" / "appdata";
    }

    return root / "CyberDeckBrowser" / "logs" / "cyberdeck.log";
}

bool Logger::Initialize(const std::filesystem::path& log_path) {
    std::error_code error;
    std::filesystem::create_directories(log_path.parent_path(), error);
    if (error) {
        return false;
    }

    RotateIfNeeded(log_path);
    stream_.open(log_path, std::ios::out | std::ios::app);
    if (!stream_.is_open()) {
        return false;
    }
    log_path_ = log_path;
    return true;
}

void Logger::Info(std::string_view message) {
    Write("INFO", message);
}

void Logger::Error(std::string_view message) {
    Write("ERROR", message);
}

std::filesystem::path Logger::path() const {
    return log_path_;
}

bool Logger::RotateIfNeeded(const std::filesystem::path& log_path) const {
    constexpr std::uintmax_t kMaximumLogBytes = 1024 * 1024;

    std::error_code error;
    if (!std::filesystem::exists(log_path, error)) {
        return true;
    }
    const std::uintmax_t size = std::filesystem::file_size(log_path, error);
    if (error || size < kMaximumLogBytes) {
        return true;
    }

    const std::filesystem::path rotated =
        log_path.parent_path() /
        (log_path.stem().wstring() + L"." + Utf8ToWide(CurrentFileTimestamp()) + log_path.extension().wstring());
    std::filesystem::rename(log_path, rotated, error);
    return !error;
}

void Logger::Write(std::string_view level, std::string_view message) {
    if (!stream_.is_open()) {
        return;
    }

    stream_ << '[' << CurrentTimestamp() << "] [" << level << "] " << message << '\n';
    stream_.flush();
}

}  // namespace cyberdeck::common
