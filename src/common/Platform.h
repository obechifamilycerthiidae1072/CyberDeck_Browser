#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace cyberdeck::common {

std::filesystem::path AppDataDirectory();
bool ReplaceFile(const std::filesystem::path& source, const std::filesystem::path& destination);
std::uint32_t CurrentProcessId();
std::string WideToUtf8(std::wstring_view value);
std::wstring Utf8ToWide(std::string_view value);

}  // namespace cyberdeck::common
