#pragma once
#include "command/command.hpp"

namespace mediatool::cli {
class Lab : public Command {
	static constexpr auto name_v = std::string_view{"lab"};

	[[nodiscard]] auto get_name() const -> std::string_view final { return name_v; }
	[[nodiscard]] auto get_args() -> std::vector<klib::args::Arg> final;
	[[nodiscard]] auto execute() -> ExitCode final;
};
} // namespace mediatool::cli
