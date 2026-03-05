#pragma once
#include "command/command.hpp"
#include "klib/args/arg.hpp"
#include "klib/args/parse_result.hpp"
#include <memory>
#include <string_view>

namespace mediatool::cli {
class App {
  public:
	[[nodiscard]] auto run(int argc, char const* const* argv) -> int;

  private:
	[[nodiscard]] auto parse_args(int argc, char const* const* argv) -> klib::args::ParseResult;

	template <std::derived_from<Command>... Ts>
	void add_commands();

	void set_omdb_token();

	std::string m_omdb_token{};
	std::string_view m_title{};

	std::vector<std::unique_ptr<Command>> m_commands{};
	std::vector<std::vector<klib::args::Arg>> m_command_args{};
};
} // namespace mediatool::cli
