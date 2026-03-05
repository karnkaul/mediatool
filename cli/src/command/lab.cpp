#include "command/lab.hpp"
#include "log.hpp"

namespace mediatool::cli {
void Lab::populate_args() {}

auto Lab::execute() -> ExitCode {
	log.debug("nothing to do");
	return ExitCode::Success;
}
} // namespace mediatool::cli
