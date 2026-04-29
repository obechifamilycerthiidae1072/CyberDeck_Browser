#include <windows.h>

#include "app/Application.h"
#include "common/Logger.h"

#include <exception>
#include <string>

namespace {

std::wstring TrimCommandLineUrl(std::wstring command_line) {
    const auto first = command_line.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return {};
    }
    const auto last = command_line.find_last_not_of(L" \t\r\n");
    command_line = command_line.substr(first, last - first + 1);

    if (command_line.size() >= 2 && command_line.front() == L'"' && command_line.back() == L'"') {
        command_line = command_line.substr(1, command_line.size() - 2);
    }
    return command_line;
}

}  // namespace

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t* command_line, int show_command) {
    try {
        cyberdeck::app::Application app(instance, show_command, TrimCommandLineUrl(command_line == nullptr ? L"" : command_line));
        return app.Run();
    } catch (const std::exception& error) {
        cyberdeck::common::Logger logger;
        if (logger.Initialize(cyberdeck::common::Logger::DefaultLogPath())) {
            logger.Error(std::string("Unhandled std::exception: ") + error.what());
        }
        MessageBoxW(
            nullptr,
            L"CyberDeck Browser hit an unexpected startup error. Details were written to the diagnostic log.",
            L"CyberDeck Browser",
            MB_OK | MB_ICONERROR);
        return 1;
    } catch (...) {
        cyberdeck::common::Logger logger;
        if (logger.Initialize(cyberdeck::common::Logger::DefaultLogPath())) {
            logger.Error("Unhandled non-standard exception.");
        }
        MessageBoxW(
            nullptr,
            L"CyberDeck Browser hit an unexpected startup error. Details were written to the diagnostic log.",
            L"CyberDeck Browser",
            MB_OK | MB_ICONERROR);
        return 1;
    }
}
