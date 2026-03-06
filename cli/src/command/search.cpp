#include "command/search.hpp"
#include "kcurl/http.hpp"
#include "klib/args/arg.hpp"
#include "log.hpp"
#include "mediatool/omdb.hpp"
#include "serialize.hpp"
#include <print>
#include <string_view>

namespace mediatool::cli {
void Search::populate_args() {
	m_args = {
		klib::args::named_option(m_type, "t,type"),
		klib::args::named_option(m_query.season, "s,season"),
		klib::args::named_option(m_query.episode, "e,episode"),
		klib::args::positional_required(m_query.title, "title"),
	};
}

auto Search::execute() -> ExitCode {
	auto type = omdb::type_map.to_enum(m_type);

	if (m_query.episode > 0) {
		type = omdb::Type::Episode;
	} else if (m_query.season > 0) {
		type = omdb::Type::Series;
	}

	auto const result = m_omdb->search(m_query, type);
	if (!result) {
		log.error("{}", result.error().text);
		return ExitCode::OmdbServiceFailure;
	}

	std::visit([&](auto const& t) { std::println("{}", serialize(t)); }, result->payload);

	return ExitCode::Success;
}
} // namespace mediatool::cli
