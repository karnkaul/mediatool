#include "command/identify.hpp"
#include "klib/visitor.hpp"
#include "log.hpp"
#include "mediatool/media_directory.hpp"
#include "mediatool/types.hpp"
#include <cstdlib>
#include <filesystem>
#include <print>
#include <string_view>

namespace mediatool::cli {
Identify::Identify() {
	m_args = {
		klib::args::positional_required(m_directory, "directory"),
	};
}

auto Identify::execute(Instance const& /*instance*/) -> int {
	auto const path = fs::path{m_directory};
	if (!fs::is_directory(path)) {
		log.error("not a directory: '{}'", m_directory);
		return EXIT_FAILURE;
	}

	auto const media_directory = identify_media_directory(path);
	if (!media_directory) {
		log.error("unrecognized media directory: '{}'", m_directory);
		return EXIT_FAILURE;
	}

	auto const visitor = klib::Visitor{
		[](MovieDirectory const& movie) { std::println(" directory type: movie\n title: {}", movie.title); },
		[](SeasonDirectory const& season) {
			std::println(" directory type: season\n title: {}", season.title);
			if (season.id) { std::println(" id: {}", season.id->as_string_view()); }
		},
		[](SeriesDirectory const& series) { std::println(" directory type: series\n title: {}", series.title); },
	};
	std::visit(visitor, *media_directory);

	return EXIT_SUCCESS;
}
} // namespace mediatool::cli
