#pragma once
#include "detail/http_gateway.hpp"
#include "kcurl/http.hpp"
#include "klib/log.hpp"
#include "klib/ptr.hpp"
#include "mediatool/omdb.hpp"
#include <string_view>

namespace mediatool::detail {
class OmdbService : public omdb::IService {
  public:
	explicit OmdbService(HttpGateway const& gateway, std::string token);

  private:
	struct Search {
		std::string_view title{};
		std::string_view type{};
		int season{};
		int episode{};
	};

	void set_api_token(std::string token) final;
	[[nodiscard]] auto get_api_token() const -> std::string_view final { return m_token; }

	[[nodiscard]] auto search_movie(std::string_view title) const -> omdb::Result<omdb::Movie> final;
	[[nodiscard]] auto search_series(std::string_view title) const -> omdb::Result<omdb::Series> final;
	[[nodiscard]] auto search_season(std::string_view title, int season) const -> omdb::Result<omdb::Season> final;
	[[nodiscard]] auto search_episode(std::string_view title, int season, int episode) const -> omdb::Result<omdb::Episode> final;
	[[nodiscard]] auto search_generic(std::string_view title) const -> omdb::Result<dj::Json> final;

	[[nodiscard]] static auto build_request(OmdbService::Search const& search) -> kcurl::http::Request;

	[[nodiscard]] auto create_secret() const -> kcurl::http::Query;
	[[nodiscard]] auto perform_search(Search const& search) const -> kcurl::http::Result<dj::Json>;

	klib::TypedLogger<omdb::IService> m_log{};

	klib::Ptr<HttpGateway const> m_gateway{};
	std::string m_token{};
};
} // namespace mediatool::detail
