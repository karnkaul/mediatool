#include "command/identify.hpp"
#include "log.hpp"
#include "mediatool/entity.hpp"
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
	auto const entity = identify_entity(m_directory);
	if (!entity) {
		log.error("unrecognized path: '{}'", m_directory);
		return EXIT_FAILURE;
	}

	auto const entry_type = entry_type_name_map.to_name(entity->entry_type);
	auto const media_type = media_type_name_map.to_name(entity->media_type);
	std::println(" entry type: {}\n media type: {}\n title: {}", entry_type, media_type, entity->title);

	return EXIT_SUCCESS;
}
} // namespace mediatool::cli
