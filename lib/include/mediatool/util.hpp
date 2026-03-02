#pragma once
#include "klib/c_string.hpp"
#include "mediatool/types.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace mediatool::util {
namespace fs = std::filesystem;

[[nodiscard]] auto get_env_var(klib::CString key) -> klib::CString;

[[nodiscard]] auto to_int(std::string_view text, int fallback = 0) -> int;

[[nodiscard]] auto identify_media_type(fs::path const& path) -> std::optional<MediaType>;
[[nodiscard]] auto extract_season_id(std::string const& name) -> std::optional<SeasonId>;
[[nodiscard]] auto extract_episode_id(std::string const& name) -> std::optional<EpisodeId>;
} // namespace mediatool::util
