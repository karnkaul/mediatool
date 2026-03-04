#include "mediatool/manifest.hpp"
#include "detail/identity.hpp"
#include "mediatool/types.hpp"
#include "mediatool/util.hpp"
#include <filesystem>
#include <utility>

namespace mediatool::detail {
namespace {
[[nodiscard]] auto largest_video_file_in(fs::path const& directory) -> fs::path {
	struct Candidate {
		fs::path path{};
		std::uint64_t size{};
	};
	auto ret = Candidate{};
	for (auto const& it : fs::recursive_directory_iterator{directory}) {
		if (!it.is_regular_file() || !util::is_video_file(it.path().extension().string())) { continue; }
		auto const size = std::uint64_t(fs::file_size(it.path()));
		if (size >= ret.size) { ret = Candidate{.path = it.path(), .size = size}; }
	}
	return std::move(ret.path);
}

// TODO: collect subs

void collect_episodes(fs::path const& directory, std::vector<Episode>& out) {
	for (auto const& it : fs::directory_iterator{directory}) {
		if (!it.is_regular_file() || !util::is_video_file(it.path().extension().string())) { continue; }
		auto id = util::extract_episode_id(it.path().stem().string());
		if (!id) { continue; }
		out.push_back(Episode{.id = std::move(*id), .path = it.path()});
	}
}

void collect_seasons(fs::path const& directory, std::vector<Season>& out) {
	for (auto const& it : fs::directory_iterator{directory}) {
		if (!it.is_directory() || !util::is_season_directory(it.path())) { continue; }
		auto id = util::extract_season_id(it.path().stem().string());
		if (!id) { continue; }
		auto& season = out.emplace_back(Season{.id = std::move(*id)});
		collect_episodes(it.path(), season.episodes);
	}
}

[[nodiscard]] auto build_movie_manifest(Identity identity) -> Manifest {
	auto ret = MovieManifest{.title = std::move(identity.title)};
	switch (identity.entry_type) {
	case EntryType::Directory: {
		ret.directory = std::move(identity.path);
		ret.movie.path = largest_video_file_in(ret.directory);
		break;
	}
	case EntryType::File: ret.movie.path = std::move(identity.path); break;
	default: return {};
	}
	return ret;
}

[[nodiscard]] auto build_episode_manifest(Identity identity) -> std::optional<Manifest> {
	auto directory = fs::path{};
	auto video = fs::path{};
	switch (identity.entry_type) {
	case EntryType::Directory: {
		directory = std::move(identity.path);
		video = largest_video_file_in(directory);
		break;
	}
	case EntryType::File: video = std::move(identity.path); break;
	default: return {};
	}

	auto id = util::extract_episode_id(video.stem().string());
	if (!id) { return {}; }
	auto episode = Episode{.id = std::move(*id), .path = std::move(video)};
	return EpisodeManifest{.episode = std::move(episode), .title = std::move(identity.title), .directory = std::move(directory)};
}

[[nodiscard]] auto build_season_manifest(Identity identity) -> std::optional<Manifest> {
	auto id = util::extract_season_id(identity.path.filename().string());
	if (!id) { return {}; }
	auto season = Season{.id = std::move(*id), .path = std::move(identity.path)};
	collect_episodes(season.path, season.episodes);
	return SeasonManifest{.season = std::move(season), .title = std::move(identity.title)};
}

[[nodiscard]] auto build_series_manifest(Identity identity) -> Manifest {
	auto series = Series{.path = std::move(identity.path)};
	collect_seasons(series.path, series.seasons);
	return SeriesManifest{.series = std::move(series), .title = std::move(identity.title)};
}

[[nodiscard]] auto build_directory_manifest(Identity identity) -> std::optional<Manifest> {
	switch (identity.media_type) {
	case MediaType::Movie: return build_movie_manifest(std::move(identity));
	case MediaType::Episode: return build_episode_manifest(std::move(identity));
	case MediaType::Season: return build_season_manifest(std::move(identity));
	case MediaType::Series: return build_series_manifest(std::move(identity));
	default: return {};
	}
}

[[nodiscard]] auto build_file_manifest(Identity identity) -> std::optional<Manifest> {
	switch (identity.media_type) {
	case MediaType::Movie: return build_movie_manifest(std::move(identity));
	case MediaType::Episode: return build_episode_manifest(std::move(identity));
	default: return {};
	}
}

auto build_manifest(fs::path const& path) -> std::optional<Manifest> {
	return detail::identify(path).and_then([&](Identity identity) {
		switch (identity.entry_type) {
		case EntryType::Directory: return build_directory_manifest(std::move(identity));
		case EntryType::File: return build_file_manifest(std::move(identity));
		default: std::unreachable();
		}
	});
}
} // namespace
} // namespace mediatool::detail

auto mediatool::build_manifest(fs::path const& path) -> std::optional<Manifest> { return detail::build_manifest(path); }
