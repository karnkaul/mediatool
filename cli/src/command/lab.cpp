#include "command/lab.hpp"
#include "klib/visitor.hpp"
#include "log.hpp"
#include "mediatool/manifest.hpp"
#include <cstdlib>
#include <filesystem>
#include <print>
#include <string_view>

namespace mediatool::cli {
namespace fs = std::filesystem;

namespace {
void print_episode_files(std::string_view const header, std::span<Episode const> episodes) {
	std::println("{}", header);
	for (auto const& episode : episodes) { std::println(" {} - {}", episode.id.as_string_view(), episode.path.filename().generic_string()); }
}
} // namespace

Lab::Lab() {
	m_args = {
		klib::args::positional_required(m_path, "path"),
	};
}

auto Lab::execute(Instance const& /*instance*/) -> int {
	auto const manifest = build_manifest(m_path);
	if (!manifest) {
		log.error("failed to build manifest for: '{}'", m_path);
		return EXIT_FAILURE;
	}

	auto const visitor = klib::Visitor{
		[](MovieManifest const& manifest) {
			std::println(" media type: movie\n video: {}\n title: {}\n directory: {}", manifest.movie.path.generic_string(), manifest.title,
						 manifest.directory.generic_string());
		},
		[](EpisodeManifest const& manifest) {
			std::println(" media type: episode\n video: {}\n title: {} \n directory: {}", manifest.episode.path.generic_string(), manifest.title,
						 manifest.directory.generic_string());
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
