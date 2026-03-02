#include "command/search.hpp"
#include "djson/json.hpp"
#include "kcurl/http.hpp"
#include "klib/args/arg.hpp"
#include "klib/visitor.hpp"
#include "log.hpp"
#include "mediatool/instance.hpp"
#include "mediatool/omdb.hpp"
#include <cstdlib>
#include <string_view>

namespace mediatool::cli {
namespace http = kcurl::http;

namespace {

auto handle_error(http::Error const& error) -> int {
	log.error("{}", error.text);
	return EXIT_FAILURE;
}
} // namespace

Search::Search() {
	m_args = {
		klib::args::named_option(m_type, "t,type"),
		klib::args::named_option(m_query.season, "s,season"),
		klib::args::named_option(m_query.episode, "e,episode"),
		klib::args::positional_required(m_query.title, "title"),
	};
}

auto Search::execute(Instance const& instance) -> int {
	auto type = omdb::type_map.to_enum(m_type);

	if (m_query.episode > 0) {
		type = omdb::Type::Episode;
	} else if (m_query.season > 0) {
		type = omdb::Type::Series;
	}

	auto const& omdb = instance.get_omdb_service();
	auto const result = omdb.search(m_query, type);
	if (!result) { return handle_error(result.error()); }

	auto const visitor = klib::Visitor{
		[&](omdb::Movie const& movie) {
			log.info("movie:\n title: {}\n year: {}\n imdb_id: {}\n plot: {}\n", movie.title, movie.year, movie.imdb_id, movie.plot);
		},
		[&](omdb::Episode const& episode) {
			log.info("episode:\n title: {}\n number: {}\n imdb_id: {}\n plot: {}\n", episode.title, episode.number, episode.imdb_id, episode.plot);
		},
		[&](omdb::Season const& season) {
			auto text = std::string{"season:\n"};
			std::format_to(std::back_inserter(text), " title: {}\n number: {}\n episodes:\n", season.title, season.number);
			for (auto const& episode : season.episodes) {
				std::format_to(std::back_inserter(text), "  S{:02}E{:02} - {}\n", m_query.season, episode.number, episode.title);
			}
			log.info("{}", text);
		},
		[&](omdb::Series const& series) {
			log.info("response:\n title: {}\n year: {}\n imdb_id: {}\n total seasons: {}\n plot: {}\n", series.title, series.year, series.imdb_id,
					 series.total_seasons, series.plot);
		},
		[&](dj::Json const& json) { log.info("response:\n{}", json.serialize()); },
	};
	std::visit(visitor, result->payload);

	return EXIT_SUCCESS;
}
} // namespace mediatool::cli
