#pragma once
#include "command/command.hpp"
#include "mediatool/omdb.hpp"
#include <string_view>

namespace mediatool::cli {
class Search : public Command {
	static constexpr auto name_v = std::string_view{"search"};

	[[nodiscard]] auto get_name() const -> std::string_view final { return name_v; }
	[[nodiscard]] auto get_args() -> std::vector<klib::args::Arg> final;
	[[nodiscard]] auto execute() -> ExitCode final;

	omdb::Query m_query{};
	std::string_view m_type{};
};
} // namespace mediatool::cli
