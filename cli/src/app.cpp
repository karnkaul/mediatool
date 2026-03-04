#include "app.hpp"
#include "command/identify.hpp"
#include "command/lab.hpp"
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
#include "mediatool/util.hpp"
#include <cstdlib>
#include <string_view>

namespace mediatool::cli {
namespace {
constexpr auto env_omdb_token_key_v = klib::CString{"MEDIATOOL_OMDB_TOKEN"};
} // namespace

auto App::run(int argc, char const* const* argv) -> int {
	m_commands.push_back(std::make_unique<Search>());
	m_commands.push_back(std::make_unique<Identify>());
	m_commands.push_back(std::make_unique<Lab>());

	auto const parse_result = parse_args(argc, argv);
	if (parse_result.early_return()) { return parse_result.get_return_code(); }

	log.debug("{}", build_version_v);

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
	auto args = std::vector{
		klib::args::named_option(m_omdb_token, "o,omdb-token", "omdb API token"),
	};
	for (auto const& command : m_commands) { args.push_back(klib::args::command(command->get_args(), command->get_name())); }
	return klib::args::parse_main(parse_info, args, argc, argv);
}

void App::set_omdb_token() {
	if (!m_omdb_token.empty()) { return; }

	log.debug("reading omdb API token from env:{}", env_omdb_token_key_v.as_view());
	auto const omdb_token = util::get_env_var(env_omdb_token_key_v);
	if (omdb_token.as_view().empty()) { throw Panic{"invalid (empty) ombd API token"}; }
	m_omdb_token = omdb_token.as_view();
}

void App::create_instance() {
	auto const get_token = [this] -> std::string_view {
		set_omdb_token();
		return m_omdb_token;
	};
	m_instance = Instance::create(m_instance_ci, get_token);
}
} // namespace mediatool::cli
