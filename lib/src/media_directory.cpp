#include "mediatool/media_directory.hpp"
#include "klib/assert.hpp"
#include "mediatool/util.hpp"
#include <regex>

namespace mediatool {
namespace {
[[nodiscard]] auto is_season_dir(fs::path const& path) {
	static auto const s_regex_1 = std::regex{R"(.*Season.[0-9]{2}.*)"};
	static auto const s_regex_2 = std::regex{R"(.*S[0-9]{2}.*)"};
	auto const filename = path.stem().string();
	return std::regex_match(filename, s_regex_1) || std::regex_match(filename, s_regex_2);
}

[[nodiscard]] auto is_episode(fs::path const& path) {
	static auto const s_regex = std::regex{R"(.*S[0-9]{2}E[0-9]{2}.*)"};
	return std::regex_match(path.string(), s_regex);
}

[[nodiscard]] auto identify_directory_type(fs::path const& path) -> std::optional<MediaType> {
	KLIB_ASSERT(fs::is_directory(path));

	auto const str = path.string();
	if (is_season_dir(path)) { return MediaType::Season; }

	auto has_video_file = false;
	for (auto const& it : fs::directory_iterator{path}) {
		if (it.is_directory() && is_season_dir(it.path())) { return MediaType::Series; }

		if (!it.is_regular_file()) { continue; }
		auto const extension = it.path().extension().string();
		if (!util::is_video_file(extension)) { continue; }

		has_video_file = true;
		if (is_episode(it.path())) { return MediaType::Season; }
	}

	if (has_video_file) { return MediaType::Movie; }

	return {};
}

[[nodiscard]] auto identify_file_type(fs::path const& path) -> std::optional<MediaType> {
	KLIB_ASSERT(fs::is_regular_file(path));
	if (!util::is_video_file(path.extension().string())) { return {}; }
	if (is_episode(path)) { return MediaType::Episode; }
	return MediaType::Movie;
}

[[nodiscard]] auto identify_media_type(fs::path const& path) -> std::optional<MediaType> {
	if (fs::is_directory(path)) { return identify_directory_type(path); }
	if (fs::is_regular_file(path)) { return identify_file_type(path); }
	return {};
}

[[nodiscard]] auto identify_directory(fs::path path, MediaType const media_type) -> std::optional<MediaDirectory> {
	auto title = util::identify_title(path);
	switch (media_type) {
	case MediaType::Movie: return MovieDirectory{.path = std::move(path), .title = std::move(title)};
	case MediaType::Episode: {
		auto title = util::identify_title(path);
		auto id = util::extract_episode_id(path.stem().string());
		return EpisodeDirectory{.id = std::move(id), .path = std::move(path), .title = std::move(title)};
	}
	case MediaType::Season: {
		auto title = util::identify_title(path);
		auto id = util::extract_season_id(path.stem().string());
		return SeasonDirectory{.id = std::move(id), .path = std::move(path), .title = std::move(title)};
	}
	case MediaType::Series: return SeriesDirectory{.path = std::move(path), .title = std::move(title)};

	default: break;
	}
	return {};
}
} // namespace
} // namespace mediatool

auto mediatool::identify_media_directory(fs::path path) -> std::optional<MediaDirectory> {
	if (!fs::is_directory(path)) { return {}; }
	return identify_media_type(path).and_then([&](MediaType const media_type) { return identify_directory(std::move(path), media_type); });
}
