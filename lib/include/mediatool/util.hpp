#pragma once
#include "klib/c_string.hpp"
#include "mediatool/media_directory.hpp"
#include "mediatool/types.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace mediatool::util {
namespace fs = std::filesystem;

[[nodiscard]] auto get_env_var(klib::CString key) -> klib::CString;

[[nodiscard]] auto to_int(std::string_view text, int fallback = 0) -> int;

[[nodiscard]] auto identify_media_type(fs::path const& path) -> std::optional<MediaType>;
[[nodiscard]] auto identify_title(fs::path const& path) -> std::string;
[[nodiscard]] auto extract_season_id(std::string const& name) -> std::optional<SeasonId>;
[[nodiscard]] auto extract_episode_id(std::string const& name) -> std::optional<EpisodeId>;

[[nodiscard]] auto identify_media_directory(fs::path const& path) -> std::optional<MediaDirectory>;
} // namespace mediatool::util
