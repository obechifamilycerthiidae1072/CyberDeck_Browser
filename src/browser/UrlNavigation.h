#pragma once

#include <string>
#include <string_view>

namespace cyberdeck::browser {

enum class NavigationDecision {
    kNavigate,
    kBlocked,
    kEmpty,
};

struct NormalizedNavigation {
    NavigationDecision decision = NavigationDecision::kEmpty;
    std::wstring target_url;
    std::wstring reason;
};

enum class ProtocolAction {
    kAllow,
    kBlock,
    kConfirmExternal,
};

struct ProtocolDecision {
    ProtocolAction action = ProtocolAction::kBlock;
    std::wstring scheme;
    std::wstring reason;
};

NormalizedNavigation NormalizeAddressBarInput(std::wstring_view input);
ProtocolDecision ClassifyNavigationProtocol(std::wstring_view url);

}  // namespace cyberdeck::browser
