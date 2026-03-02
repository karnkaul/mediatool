#include "mediatool/util.hpp"
#include "mediatool/types.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <regex>
#include <string_view>

namespace mediatool {
namespace util {
namespace {
using namespace std::string_view_literals;

constexpr auto video_extensions_v = std::array{
	".mp4"sv, ".mkv"sv, ".avi"sv, ".m4v"sv, ".webm"sv,
};

constexpr auto is_video_file(std::string_view const extension) { return std::ranges::find(video_extensions_v, extension) != video_extensions_v.end(); }

[[nodiscard]] auto is_season_dir(fs::path const& path) {
	static auto const regex = std::regex{R"(.*Season.[0-9]{2}.*)"};
	return std::regex_match(path.filename().string(), regex);
}

[[nodiscard]] auto is_episode(fs::path const& path) {
	static auto const regex = std::regex{R"(.*S[0-9]{2}E[0-9]{2}.*)"};
	return std::regex_match(path.string(), regex);
}
} // namespace
} // namespace util

auto util::get_env_var(klib::CString const key) -> klib::CString {
	if (key.as_view().empty()) { return {}; }
	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	return std::getenv(key.c_str());
}

auto util::identify_directory_type(fs::path const& path) -> DirectoryType {
	if (!fs::is_directory(path)) { return DirectoryType::Unknown; }

	auto const str = path.string();
	if (is_season_dir(path)) { return DirectoryType::Season; }

	auto has_video_file = false;
	for (auto const& it : fs::directory_iterator{path}) {
		if (it.is_directory() && is_season_dir(it.path())) { return DirectoryType::Series; }

		if (!it.is_regular_file()) { continue; }
		auto const extension = it.path().extension().string();
		if (!is_video_file(extension)) { continue; }

		has_video_file = true;
		if (is_episode(it.path())) { return DirectoryType::Season; }
	}

	if (has_video_file) { return DirectoryType::Movie; }

	return DirectoryType::Unknown;
}
} // namespace mediatool
