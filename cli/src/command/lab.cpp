#include "command/lab.hpp"
#include "log.hpp"

namespace mediatool::cli {
auto Lab::get_args() -> std::vector<klib::args::Arg> { return {}; }

auto Lab::execute() -> ExitCode {
	log.debug("nothing to do");
	return ExitCode::Success;
}
} // namespace mediatool::cli
