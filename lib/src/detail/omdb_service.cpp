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

using omdb::MediaType;

namespace {
constexpr auto url_v = std::string_view{"http://www.omdbapi.com/"};

namespace key {
constexpr auto apikey_v = std::string_view{"apikey"};
constexpr auto type_v = std::string_view{"type"};
constexpr auto title_v = std::string_view{"t"};
constexpr auto season_v = std::string_view{"Season"};
constexpr auto episode_v = std::string_view{"Episode"};
} // namespace key

[[nodiscard]] constexpr auto is_valid(MediaType const type) { return type >= MediaType{0} && type < MediaType::COUNT_; }

struct RequestBuilder {
	auto add_query(std::string_view const key, std::string value) -> RequestBuilder& {
		if (!value.empty()) { request.queries.push_back(http::Query{.key = std::string{key}, .value = std::move(value)}); }
		return *this;
	}

	auto add_type(std::string_view const type) -> RequestBuilder& {
		if (type.empty()) { return *this; }
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
[[nodiscard]] auto to_payload(http::Response<dj::Json> const& response) -> http::Response<omdb::Payload> {
	auto ret = Type{};
	from_json(response.payload, ret);
	return response.rewrap_as(omdb::Payload{std::move(ret)});
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

auto OmdbService::search(Query const& query, std::optional<MediaType> const type) const -> omdb::Result<omdb::Payload> {
	if (!type || !is_valid(*type)) {
		return perform_search(query, {}).transform([](http::Response<dj::Json> in) { return in.rewrap_as(omdb::Payload{std::move(in.payload)}); });
	}

	auto const type_str = omdb::media_type_str_v[*type];
	switch (*type) {
	case MediaType::Movie: return perform_search(query, type_str).transform(&to_payload<omdb::Movie>);
	case MediaType::Series: {
		if (query.season > 0) { return perform_search(query, type_str).transform(&to_payload<omdb::Season>); }
		return perform_search(query, type_str).transform(&to_payload<omdb::Series>);
	}
	case MediaType::Episode: return perform_search(query, type_str).transform(&to_payload<omdb::Episode>);
	default: std::unreachable();
	}
}

auto OmdbService::build_request(Query const& query, std::string_view const type) -> http::Request {
	return RequestBuilder{}.add_type(type).add_title(query.title).add_season(query.season).add_episode(query.episode).build();
}

auto OmdbService::create_secret() const -> http::Query { return http::Query{.key = std::string{key::apikey_v}, .value = m_token}; }

auto OmdbService::perform_search(Query const& query, std::string_view const type) const -> http::Result<dj::Json> {
	return m_gateway->get_json(build_request(query, type), create_secret());
}
} // namespace mediatool::detail
