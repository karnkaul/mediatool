#pragma once
#include "klib/c_string.hpp"
#include "mediatool/types.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <optional>
#include <string>

namespace mediatool::util {
namespace fs = std::filesystem;

[[nodiscard]] auto get_env_var(klib::CString key) -> klib::CString;

[[nodiscard]] auto to_int(std::string_view text, int fallback = 0) -> int;

constexpr auto video_extensions_v = std::array{
	".mp4", ".mkv", ".avi", ".m4v", ".webm",
};
constexpr auto is_video_file(std::string_view const extension) { return std::ranges::find(video_extensions_v, extension) != video_extensions_v.end(); }

constexpr auto subtitle_extensions_v = std::array{
	".srt",
};
constexpr auto is_subtitle_file(std::string_view const extension) { return std::ranges::find(subtitle_extensions_v, extension) != subtitle_extensions_v.end(); }

[[nodiscard]] auto is_season_directory(fs::path const& path) -> bool;
[[nodiscard]] auto is_episode(fs::path const& path) -> bool;

[[nodiscard]] auto identify_title(fs::path const& path) -> std::string;
[[nodiscard]] auto extract_season_id(std::string const& name) -> std::optional<SeasonId>;
[[nodiscard]] auto extract_episode_id(std::string const& name) -> std::optional<EpisodeId>;
} // namespace mediatool::util
