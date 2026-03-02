#include "command/identify.hpp"
#include "log.hpp"
#include "mediatool/types.hpp"
#include "mediatool/util.hpp"
#include <cstdlib>
#include <filesystem>
#include <string_view>

namespace mediatool::cli {
namespace fs = std::filesystem;

Identify::Identify() {
	m_args = {
		klib::args::positional_required(m_path, "path"),
	};
}

auto Identify::execute(Instance const& /*instance*/) -> int {
	auto const path = fs::path{m_path};
	auto const media_type = util::identify_media_type(path);
	if (!media_type) {
		log.error("unknown MediaType: '{}'", m_path);
		return EXIT_FAILURE;
	}

	std::string_view const entry_type = fs::is_directory(path) ? "directory" : "file";
	log.info("MediaType: {} ({})", media_type_str_v[*media_type], entry_type);
	return EXIT_SUCCESS;
}
} // namespace mediatool::cli
