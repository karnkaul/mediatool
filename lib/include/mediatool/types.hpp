#pragma once
#include "klib/enum_array.hpp"
#include <cstdint>
#include <string_view>

namespace mediatool {
enum class DirectoryType : std::int8_t { Unknown, Movie, Season, Series, COUNT_ };
constexpr auto directory_type_str_v = klib::EnumArray<DirectoryType, std::string_view>{"unknown", "movie", "season", "series"};
} // namespace mediatool
