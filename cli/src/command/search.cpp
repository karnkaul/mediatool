#include "command/search.hpp"
#include "kcurl/http.hpp"
#include "klib/args/arg.hpp"
#include "log.hpp"
#include "mediatool/instance.hpp"
#include "mediatool/omdb.hpp"
#include <cstdlib>
#include <string_view>
#include <utility>

namespace mediatool::cli {
namespace http = kcurl::http;

namespace {
[[nodiscard]] constexpr auto to_media_type(std::string_view const in) -> omdb::MediaType {
	for (auto i = std::to_underlying(omdb::MediaType::None); i < std::to_underlying(omdb::MediaType::COUNT_); ++i) {
		auto const mt = omdb::MediaType{i};
		if (omdb::media_type_str_v[mt] == in) { return mt; }
	}
	return omdb::MediaType::None;
}

auto handle_error(http::Error const& error) -> int {
	log.error("{}", error.text);
	return EXIT_FAILURE;
}
} // namespace

Search::Search() {
	m_args = {
		klib::args::named_option(m_media_type, "t,type"),
		klib::args::named_option(m_season, "s,season"),
		klib::args::named_option(m_episode, "e,episode"),
		klib::args::positional_required(m_title, "title"),
	};
}

auto Search::execute(Instance const& instance) -> int {
	auto media_type = to_media_type(m_media_type);

	if (m_episode > 0) {
		media_type = omdb::MediaType::Episode;
	} else if (m_season > 0) {
		media_type = omdb::MediaType::Series;
	}

	auto const& omdb = instance.get_omdb_service();
	switch (media_type) {
	case omdb::MediaType::Movie: return search_movie(omdb);
	case omdb::MediaType::Series: {
		if (m_season > 0) { return search_season(omdb); }
		return search_series(omdb);
	}
	case omdb::MediaType::Episode: return search_episode(omdb);
	case omdb::MediaType::None: return search_generic(omdb);
	default: {
		log.error("unrecognized media type: '{}'", std::to_underlying(media_type));
		return EXIT_FAILURE;
	}
	}
}

auto Search::search_episode(omdb::IService const& omdb) const -> int {
	if (m_season <= 0 || m_episode <= 0) {
		log.error("invalid season/episode: '{}' / '{}'", m_season, m_episode);
		return EXIT_FAILURE;
	}

	log.info("searching episode: {} S{:02}E{:02}", m_title, m_season, m_episode);
	auto result = omdb.search_episode(m_title, m_season, m_episode);
	if (!result) { return handle_error(result.error()); }

	auto const& episode = result->payload;
	log.info("episode:\n title: {}\n number: {}\n imdb_id: {}\n plot: {}\n", episode.title, episode.number, episode.imdb_id, episode.plot);

	return EXIT_SUCCESS;
}

auto Search::search_season(omdb::IService const& omdb) const -> int {
	if (m_season <= 0) {
		log.error("invalid season: '{}'", m_season);
		return EXIT_FAILURE;
	}

	log.info("searching season: {} S{:02}", m_title, m_season);
	auto result = omdb.search_season(m_title, m_season);
	if (!result) { return handle_error(result.error()); }

	auto const& season = result->payload;
	auto text = std::string{"season:\n"};
	std::format_to(std::back_inserter(text), " title: {}\n number: {}\n episodes:\n", season.title, season.number);
	for (auto const& episode : season.episodes) { std::format_to(std::back_inserter(text), "  S{:02}E{:02} - {}\n", m_season, episode.number, episode.title); }
	log.info("{}", text);

	return EXIT_SUCCESS;
}

auto Search::search_series(omdb::IService const& omdb) const -> int {
	auto result = omdb.search_series(m_title);
	if (!result) { return handle_error(result.error()); }

	log.info("searching series: {}", m_title);
	auto const& series = result->payload;
	log.info("response:\n title: {}\n year: {}\n imdb_id: {}\n total seasons: {}\n plot: {}\n", series.title, series.year, series.imdb_id, series.total_seasons,
			 series.plot);

	return EXIT_SUCCESS;
}

auto Search::search_movie(omdb::IService const& omdb) const -> int {
	log.info("searching movie: {}", m_title);
	auto result = omdb.search_movie(m_title);
	if (!result) { return handle_error(result.error()); }

	auto const& movie = result->payload;
	log.info("movie:\n title: {}\n year: {}\n imdb_id: {}\n plot: {}\n", movie.title, movie.year, movie.imdb_id, movie.plot);

	return EXIT_SUCCESS;
}

auto Search::search_generic(omdb::IService const& omdb) const -> int {
	log.info("searching generic: {}", m_title);
	auto result = omdb.search_generic(m_title);
	if (!result) { return handle_error(result.error()); }

	auto const& json = result->payload;
	log.info("response:\n{}", json.serialize());

	return EXIT_SUCCESS;
}
} // namespace mediatool::cli
