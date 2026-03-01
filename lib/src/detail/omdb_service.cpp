#include "detail/omdb_service.hpp"
#include "detail/http_gateway.hpp"
#include "detail/json_io.hpp"
#include "djson/json.hpp"
#include "kcurl/http.hpp"
#include "mediatool/omdb.hpp"
#include "mediatool/panic.hpp"
#include <string>
#include <string_view>

namespace mediatool::detail {
namespace http = kcurl::http;

namespace {
constexpr auto url_v = std::string_view{"http://www.omdbapi.com/"};

namespace key {
constexpr auto apikey_v = std::string_view{"apikey"};
constexpr auto type_v = std::string_view{"type"};
constexpr auto title_v = std::string_view{"t"};
constexpr auto season_v = std::string_view{"Season"};
constexpr auto episode_v = std::string_view{"Episode"};
} // namespace key

struct RequestBuilder {
	auto add_query(std::string_view const key, std::string value) -> RequestBuilder& {
		if (!value.empty()) { request.queries.push_back(http::Query{.key = std::string{key}, .value = std::move(value)}); }
		return *this;
	}

	auto add_type(std::string_view const type) -> RequestBuilder& {
		if (type.empty() || type == omdb::media_type_str_v[omdb::MediaType::None]) { return *this; }
		return add_query(key::type_v, std::string{type});
	}

	auto add_title(std::string_view const title) -> RequestBuilder& { return add_query(key::title_v, http::escape(title)); }

	auto add_season(int const season) -> RequestBuilder& {
		if (season <= 0) { return *this; }
		return add_query(key::season_v, std::format("{}", season));
	}

	auto add_episode(int const episode) -> RequestBuilder& {
		if (episode <= 0) { return *this; }
		return add_query(key::episode_v, std::format("{}", episode));
	}

	auto build() -> http::Request { return std::move(request); }

	http::Request request{.base_url = std::string{url_v}};
};

template <typename Type>
[[nodiscard]] auto to(http::Response<dj::Json> const& response) -> http::Response<Type> {
	auto ret = Type{};
	from_json(response.payload, ret);
	return response.rewrap_as(std::move(ret));
}
} // namespace

OmdbService::OmdbService(HttpGateway const& gateway, std::string token) : m_gateway(&gateway) {
	if (token.empty()) { throw Panic{"invalid (empty) API token"}; }
	set_api_token(std::move(token));
}

void OmdbService::set_api_token(std::string token) {
	if (token.empty()) {
		m_log.warn("attempting to set empty API token");
		return;
	}

	m_token = std::move(token);
	m_log.debug("API token changed");
}

auto OmdbService::search_movie(std::string_view const title) const -> http::Result<omdb::Movie> {
	auto const search = OmdbService::Search{
		.title = title,
		.type = omdb::media_type_str_v[omdb::MediaType::Movie],
	};
	return perform_search(search).transform(&to<omdb::Movie>);
}

auto OmdbService::search_series(std::string_view title) const -> http::Result<omdb::Series> {
	auto const search = OmdbService::Search{
		.title = title,
		.type = omdb::media_type_str_v[omdb::MediaType::Series],
	};
	return perform_search(search).transform(&to<omdb::Series>);
}

auto OmdbService::search_season(std::string_view const title, int const season) const -> http::Result<omdb::Season> {
	auto const search = OmdbService::Search{
		.title = title,
		.type = omdb::media_type_str_v[omdb::MediaType::Series],
		.season = season,
	};
	return perform_search(search).transform(&to<omdb::Season>);
}

auto OmdbService::search_episode(std::string_view const title, int const season, int const episode) const -> http::Result<omdb::Episode> {
	auto const search = OmdbService::Search{
		.title = title,
		.type = omdb::media_type_str_v[omdb::MediaType::Episode],
		.season = season,
		.episode = episode,
	};
	return perform_search(search).transform(&to<omdb::Episode>);
}

auto OmdbService::search_generic(std::string_view const title) const -> http::Result<dj::Json> {
	auto const search = OmdbService::Search{
		.title = title,
		.type = omdb::media_type_str_v[omdb::MediaType::None],
	};
	return perform_search(search);
}

auto OmdbService::build_request(OmdbService::Search const& search) -> http::Request {
	return RequestBuilder{}.add_type(search.type).add_title(search.title).add_season(search.season).add_episode(search.episode).build();
}

auto OmdbService::create_secret() const -> http::Query { return http::Query{.key = std::string{key::apikey_v}, .value = m_token}; }

auto OmdbService::perform_search(Search const& search) const -> http::Result<dj::Json> { return m_gateway->get_json(build_request(search), create_secret()); }
} // namespace mediatool::detail
