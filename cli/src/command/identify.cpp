#include "command/identify.hpp"
#include "log.hpp"
#include "mediatool/types.hpp"
#include "mediatool/util.hpp"
#include <cstdlib>

namespace mediatool::cli {
Identify::Identify() {
	m_args = {
		klib::args::positional_required(m_directory, "directory"),
	};
}

auto Identify::execute(Instance const& /*instance*/) -> int {
	auto const result = util::identify_directory_type(m_directory);
	log.info("directory type: {}", directory_type_str_v[result]);
	return result == DirectoryType::Unknown ? EXIT_FAILURE : EXIT_SUCCESS;
}
} // namespace mediatool::cli
