#pragma once
#include "command/command.hpp"
#include "klib/args/parse_result.hpp"
#include "mediatool/instance.hpp"
#include <memory>
#include <string_view>

namespace mediatool::cli {
class App {
  public:
	[[nodiscard]] auto run(int argc, char const* const* argv) -> int;

  private:
	[[nodiscard]] auto parse_args(int argc, char const* const* argv) -> klib::args::ParseResult;

	void set_omdb_token();
	void create_instance();

	Instance::CreateInfo m_instance_ci{};
	std::string m_omdb_token{};
	std::string_view m_title{};

	std::unique_ptr<Instance> m_instance{};

	std::vector<std::unique_ptr<Command>> m_commands{};
};
} // namespace mediatool::cli
