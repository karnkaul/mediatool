#include "mediatool/manifest.hpp"
#include "detail/identity.hpp"
#include "detail/media_file.hpp"
#include "mediatool/types.hpp"
#include "mediatool/util.hpp"
#include <cstdint>
#include <filesystem>
#include <string_view>
#include <utility>

namespace mediatool::detail {
namespace {
class Builder {
  public:
	[[nodiscard]] auto build(fs::path const& path) -> std::optional<Manifest> {
		return detail::identify(path).and_then([this](Identity identity) { return build(std::move(identity)); });
	}

  private:
	struct Directory {
		[[nodiscard]] static auto build(fs::path const& path) -> Directory {
			auto ret = Directory{};
			ret.videos = collect_media_files(path);
			auto const transfer = [&](MediaFile& file) {
				if (file.type == MediaFile::Type::Subtitle) {
					ret.subtitles.push_back(std::move(file.path));
					return true;
				}
				return false;
			};
			std::erase_if(ret.videos, transfer);
			return ret;
		}

		std::vector<MediaFile> videos{};
		std::vector<fs::path> subtitles{};
	};

	[[nodiscard]] auto largest_video_file() const -> fs::path {
		struct Candidate {
			fs::path path{};
			std::uint64_t size{};
		};
		auto ret = Candidate{};
		for (auto const& media_file : m_current_dir.videos) {
			if (media_file.size >= ret.size) { ret = Candidate{.path = media_file.path, .size = media_file.size}; }
		}
		return std::move(ret.path);
	}

	void fill_subtitles(std::vector<fs::path>& out, std::string_view const stem) {
		auto const transfer = [&](fs::path& subtitle) {
			if (!subtitle.generic_string().contains(stem)) { return false; }
			out.push_back(std::move(subtitle));
			return true;
		};
		std::erase_if(m_current_dir.subtitles, transfer);
	}

	auto build(Identity identity) -> std::optional<Manifest> {
		switch (identity.entry_type) {
		case EntryType::Directory: return build_directory(std::move(identity));
		case EntryType::File: return build_file(std::move(identity));
		default: std::unreachable();
		}
	}

	auto build_file(Identity identity) -> std::optional<Manifest> {
		m_current_dir = Directory::build(identity.path);
		switch (identity.media_type) {
		case MediaType::Movie: return build_movie(std::move(identity));
		case MediaType::Episode: return build_episode(std::move(identity));
		default: return {};
		}
	}

	auto build_directory(Identity identity) -> std::optional<Manifest> {
		switch (identity.media_type) {
		case MediaType::Movie: return build_movie(std::move(identity));
		case MediaType::Episode: return build_episode(std::move(identity));
		case MediaType::Season: return build_season(std::move(identity));
		case MediaType::Series: return build_series(std::move(identity));
		default: return {};
		}
	}

	auto build_movie(Identity identity) -> Manifest {
		m_current_dir = Directory::build(identity.path);
		auto ret = MovieManifest{.title = std::move(identity.title)};
		switch (identity.entry_type) {
		case EntryType::Directory: {
			ret.directory = std::move(identity.path);
			ret.movie.path = largest_video_file();
			break;
		}
		case EntryType::File: ret.movie.path = std::move(identity.path); break;
		default: return {};
		}
		fill_subtitles(ret.movie.subtitles, ret.movie.path.stem().string());
		return ret;
	}

	auto build_episode(Identity identity) -> std::optional<Manifest> {
		m_current_dir = Directory::build(identity.path);
		auto directory = fs::path{};
		auto video = fs::path{};
		switch (identity.entry_type) {
		case EntryType::Directory: {
			directory = std::move(identity.path);
			video = largest_video_file();
			break;
		}
		case EntryType::File: video = std::move(identity.path); break;
		default: return {};
		}

		return get_episode(video).transform([&](Episode episode) {
			return EpisodeManifest{.episode = std::move(episode), .title = std::move(identity.title), .directory = std::move(directory)};
		});
	}

	auto build_season(Identity identity) -> std::optional<Manifest> {
		m_current_dir = Directory::build(identity.path);
		return get_season(std::move(identity.path)).transform([&](Season season) {
			return SeasonManifest{.season = std::move(season), .title = std::move(identity.title)};
		});
	}

	auto build_series(Identity identity) -> Manifest {
		auto series = Series{.path = std::move(identity.path)};
		for (auto const& it : fs::directory_iterator{series.path}) {
			m_current_dir = Directory::build(it.path());
			auto season = get_season(it.path());
			if (!season) { continue; }
			series.seasons.push_back(std::move(*season));
		}
		return SeriesManifest{.series = std::move(series), .title = std::move(identity.title)};
	}

	auto get_season(fs::path path) -> std::optional<Season> {
		return util::extract_season_id(path.filename().string()).transform([&](SeasonId id) {
			return Season{
				.id = std::move(id),
				.path = std::move(path),
				.episodes = get_episodes(),
			};
		});
	}

	auto get_episodes() -> std::vector<Episode> {
		auto ret = std::vector<Episode>{};
		for (auto const& media_file : m_current_dir.videos) {
			auto episode = get_episode(media_file.path);
			if (!episode) { continue; }
			ret.push_back(std::move(*episode));
		}
		return ret;
	}

	[[nodiscard]] auto get_episode(fs::path path) -> std::optional<Episode> {
		auto const stem = path.stem().string();
		return util::extract_episode_id(stem).transform([&](EpisodeId id) {
			auto ret = Episode{.id = std::move(id), .path = std::move(path)};
			fill_subtitles(ret.subtitles, stem);
			return ret;
		});
	}

	Directory m_current_dir{};
};
} // namespace
} // namespace mediatool::detail

auto mediatool::build_manifest(fs::path const& path) -> std::optional<Manifest> { return detail::Builder{}.build(fs::canonical(path)); }
