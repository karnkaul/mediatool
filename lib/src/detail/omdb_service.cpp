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

using omdb::Type;

namespace {
constexpr auto url_v = std::string_view{"http://www.omdbapi.com/"};

namespace key {
constexpr auto apikey_v = std::string_view{"apikey"};
constexpr auto type_v = std::string_view{"type"};
constexpr auto title_v = std::string_view{"t"};
constexpr auto season_v = std::string_view{"Season"};
constexpr auto episode_v = std::string_view{"Episode"};
} // namespace key

[[nodiscard]] constexpr auto is_valid(Type const type) { return type >= Type{0} && type < Type::COUNT_; }

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

OmdbService::OmdbService(HttpGateway const& gateway, IApiTokenProvider& token_provider) : m_gateway(&gateway), m_token_provider(&token_provider) {}

auto OmdbService::search(Query const& query, std::optional<Type> const type) const -> omdb::Result<omdb::Payload> {
	if (!type || !is_valid(*type)) {
		return perform_search(query, {}).transform([](http::Response<dj::Json> in) { return in.rewrap_as(omdb::Payload{std::move(in.payload)}); });
	}

	auto const type_name = omdb::type_map.to_name(*type);
	switch (*type) {
	case Type::Movie: return perform_search(query, type_name).transform(&to_payload<omdb::Movie>);
	case Type::Series: {
		if (query.season > 0) { return perform_search(query, type_name).transform(&to_payload<omdb::Season>); }
		return perform_search(query, type_name).transform(&to_payload<omdb::Series>);
	}
	case Type::Episode: return perform_search(query, type_name).transform(&to_payload<omdb::Episode>);
	default: std::unreachable();
	}
}

auto OmdbService::build_request(Query const& query, std::string_view const type) -> http::Request {
	return RequestBuilder{}.add_type(type).add_title(query.title).add_season(query.season).add_episode(query.episode).build();
}

auto OmdbService::create_secret() const -> http::Query {
	auto const token = m_token_provider->get_api_token();
	if (token.empty()) { throw Panic{"invalid (empty) omdb API token"}; }

	return http::Query{
		.key = std::string{key::apikey_v},
		.value = std::string{token},
	};
}

auto OmdbService::perform_search(Query const& query, std::string_view const type) const -> http::Result<dj::Json> {
	return m_gateway->get_json(build_request(query, type), create_secret());
}
} // namespace mediatool::detail
