#include "command/identify.hpp"
#include "klib/visitor.hpp"
#include "log.hpp"
#include "mediatool/manifest.hpp"
#include <cstdlib>
#include <filesystem>
#include <print>

namespace mediatool::cli {
namespace {
void print_subtitles(std::span<fs::path const> subtitles) {
	for (auto const& subtitle : subtitles) { std::println("  {}", subtitle.filename().generic_string()); }
}

void print_episode_files(std::string_view const header, std::span<Episode const> episodes) {
	std::println("{}", header);
	for (auto const& episode : episodes) {
		std::println(" {} - {}", episode.id.as_string_view(), episode.path.filename().generic_string());
		print_subtitles(episode.subtitles);
	}
}
} // namespace

Identify::Identify() {
	m_args = {
		klib::args::positional_required(m_path, "path"),
	};
}

auto Identify::execute(Instance const& /*instance*/) -> int {
	auto const path = fs::path{m_path};
	if (!fs::exists(path)) {
		log.error("nonexistent path: '{}'", m_path);
		return EXIT_FAILURE;
	}

	auto const manifest = build_manifest(path);
	if (!manifest) {
		log.error("failed to build manifest for: '{}'", m_path);
		return EXIT_FAILURE;
	}

	auto const visitor = klib::Visitor{
		[](MovieManifest const& manifest) {
			std::println(" media type: movie\n title: {}\n directory: {}\n video: {}", manifest.title, manifest.directory.generic_string(),
						 manifest.movie.path.generic_string());
			print_subtitles(manifest.movie.subtitles);
		},
		[](EpisodeManifest const& manifest) {
			std::println(" media type: episode\n video: {}\n title: {} \n directory: {}", manifest.episode.path.generic_string(), manifest.title,
						 manifest.directory.generic_string());
			print_subtitles(manifest.episode.subtitles);
		},
		[](SeasonManifest const& manifest) {
			std::println(" media type: season\n id: {}\n directory: {}\n title: {}", manifest.season.id.as_string_view(), manifest.season.path.generic_string(),
						 manifest.title);
			print_episode_files("episodes", manifest.season.episodes);
		},
		[](SeriesManifest const& manifest) {
			std::println(" media type: series\n directory: {}\n title: {}", manifest.series.path.generic_string(), manifest.title);
			std::println(" seasons:");
			for (auto const& season : manifest.series.seasons) { print_episode_files(season.id.as_string_view(), season.episodes); }
		},
	};
	std::visit(visitor, *manifest);

	return EXIT_SUCCESS;
}
} // namespace mediatool::cli
