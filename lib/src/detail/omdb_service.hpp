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
	explicit OmdbService(HttpGateway const& gateway, omdb::GetApiToken get_api_token);

  private:
	struct Search {
		std::string_view title{};
		std::string_view type{};
		int season{};
		int episode{};
	};

	[[nodiscard]] auto search(Query const& query, std::optional<omdb::Type> type) const -> omdb::Result<omdb::Payload> final;

	[[nodiscard]] static auto build_request(Query const& query, std::string_view type) -> kcurl::http::Request;

	[[nodiscard]] auto create_secret() const -> kcurl::http::Query;
	[[nodiscard]] auto perform_search(Query const& query, std::string_view type) const -> kcurl::http::Result<dj::Json>;

	klib::TypedLogger<omdb::IService> m_log{};

	klib::Ptr<HttpGateway const> m_gateway{};
	mutable omdb::GetApiToken m_get_api_token{};
};
} // namespace mediatool::detail
