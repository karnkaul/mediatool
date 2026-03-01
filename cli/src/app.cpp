#include "app.hpp"
#include "command/search.hpp"
#include "klib/args/arg.hpp"
#include "klib/args/parse.hpp"
#include "klib/args/parse_info.hpp"
#include "klib/c_string.hpp"
#include "klib/version_str.hpp"
#include "log.hpp"
#include "mediatool/build_version.hpp"
#include "mediatool/instance.hpp"
#include "mediatool/panic.hpp"
#include <cstdlib>
#include <print>

namespace mediatool::cli {
namespace {
constexpr auto env_omdb_token_key = klib::CString{"MEDIATOOL_OMDB_TOKEN"};
} // namespace

auto App::run(int argc, char const* const* argv) -> int {
	m_commands.push_back(std::make_unique<Search>());

	auto const parse_result = parse_args(argc, argv);
	if (parse_result.early_return()) { return parse_result.get_return_code(); }

	log.info("mediatool: {}", build_version_v);

	set_omdb_token();
	create_instance();

	auto const it = std::ranges::find_if(m_commands, [name = parse_result.get_command_name()](auto const& c) { return c->get_name() == name; });
	if (it == m_commands.end()) {
		log.error("unrecognized command: {}", parse_result.get_command_name());
		return EXIT_FAILURE;
	}

	auto& command = **it;
	return command.execute(*m_instance);
}

auto App::parse_args(int argc, char const* const* argv) -> klib::args::ParseResult {
	auto const version = std::format("{}", build_version_v);
	auto const parse_info = klib::args::ParseInfo{
		.version = version,
	};
	auto const args = std::array{
		klib::args::named_option(m_instance_ci.omdb_token, "o,omdb-token", "omdb API token"),
		klib::args::command(m_commands.front()->get_args(), m_commands.front()->get_name()),
	};
	return klib::args::parse_main(parse_info, args, argc, argv);
}

void App::set_omdb_token() {
	if (!m_instance_ci.omdb_token.empty()) { return; }

	log.debug("reading omdb token from env:{}", env_omdb_token_key.as_view());
	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	auto const* omdb_token = std::getenv(env_omdb_token_key.c_str());
	if (!omdb_token || !*omdb_token) { throw Panic{"invalid (empty) ombd API token"}; }
	m_instance_ci.omdb_token = omdb_token;
}

void App::create_instance() { m_instance = mediatool::Instance::create(m_instance_ci); }
} // namespace mediatool::cli
