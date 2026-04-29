#include "browser/UrlNavigation.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Case {
    std::wstring input;
    cyberdeck::browser::NavigationDecision decision;
    std::wstring target;
};

struct ProtocolCase {
    std::wstring url;
    cyberdeck::browser::ProtocolAction action;
    std::wstring scheme;
};

bool RunCase(const Case& test_case) {
    const auto result = cyberdeck::browser::NormalizeAddressBarInput(test_case.input);
    if (result.decision == test_case.decision && result.target_url == test_case.target) {
        return true;
    }

    std::wcerr << L"Failed input: " << test_case.input << L"\n"
               << L"  Expected target: " << test_case.target << L"\n"
               << L"  Actual target:   " << result.target_url << L"\n";
    return false;
}

bool RunProtocolCase(const ProtocolCase& test_case) {
    const auto result = cyberdeck::browser::ClassifyNavigationProtocol(test_case.url);
    if (result.action == test_case.action && result.scheme == test_case.scheme) {
        return true;
    }

    std::wcerr << L"Failed protocol input: " << test_case.url << L"\n"
               << L"  Expected scheme: " << test_case.scheme << L"\n"
               << L"  Actual scheme:   " << result.scheme << L"\n";
    return false;
}

}  // namespace

int main() {
    using cyberdeck::browser::NavigationDecision;
    using cyberdeck::browser::ProtocolAction;

    const std::vector<Case> cases{
        {L"example.com", NavigationDecision::kNavigate, L"https://example.com"},
        {L"youtube.com", NavigationDecision::kNavigate, L"https://youtube.com"},
        {L"http://example.com", NavigationDecision::kNavigate, L"http://example.com"},
        {L"https://example.com", NavigationDecision::kNavigate, L"https://example.com"},
        {L"hello world", NavigationDecision::kNavigate, L"https://duckduckgo.com/?q=hello+world"},
        {L"localhost:8080", NavigationDecision::kNavigate, L"https://localhost:8080"},
        {L"file:///C:/secret.txt", NavigationDecision::kBlocked, L""},
        {L"javascript:alert(1)", NavigationDecision::kBlocked, L""},
        {L"data:text/html,hello", NavigationDecision::kBlocked, L""},
        {L"custom-scheme://thing", NavigationDecision::kBlocked, L""},
        {L"C:\\Users\\tipp_\\Desktop\\file.html", NavigationDecision::kBlocked, L""},
    };

    const std::vector<ProtocolCase> protocol_cases{
        {L"https://example.com", ProtocolAction::kAllow, L"https"},
        {L"http://example.com", ProtocolAction::kAllow, L"http"},
        {L"about:blank", ProtocolAction::kAllow, L"about"},
        {L"mailto:test@example.com", ProtocolAction::kConfirmExternal, L"mailto"},
        {L"tel:+15551212", ProtocolAction::kConfirmExternal, L"tel"},
        {L"file:///C:/secret.txt", ProtocolAction::kBlock, L"file"},
        {L"javascript:alert(1)", ProtocolAction::kBlock, L"javascript"},
        {L"data:text/html,hello", ProtocolAction::kBlock, L"data"},
        {L"custom-scheme://thing", ProtocolAction::kConfirmExternal, L"custom-scheme"},
    };

    bool all_passed = true;
    for (const auto& test_case : cases) {
        all_passed = RunCase(test_case) && all_passed;
    }
    for (const auto& test_case : protocol_cases) {
        all_passed = RunProtocolCase(test_case) && all_passed;
    }

    return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
