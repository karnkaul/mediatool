#pragma once
#include "klib/c_string.hpp"
#include "mediatool/types.hpp"
#include <filesystem>

namespace mediatool::util {
namespace fs = std::filesystem;

[[nodiscard]] auto get_env_var(klib::CString key) -> klib::CString;

[[nodiscard]] auto identify_directory_type(fs::path const& path) -> DirectoryType;
} // namespace mediatool::util
