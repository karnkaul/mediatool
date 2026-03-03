#include "mediatool/instance.hpp"
#include "detail/http_gateway.hpp"
#include "detail/omdb_service.hpp"
#include "kcurl/curl.hpp"
#include "klib/log.hpp"
#include "mediatool/omdb.hpp"
#include <optional>

namespace mediatool {
namespace detail {
namespace {
class Instance : public mediatool::Instance {
  public:
	explicit Instance(CreateInfo const& create_info, omdb::GetApiToken get_omdb_api_token) {
		if ((create_info.flags & InstanceCreateFlag::ExternalCurl) == InstanceCreateFlag::None) { m_curl.emplace(); }
		m_omdb.emplace(m_gateway, std::move(get_omdb_api_token));
	}

  private:
	[[nodiscard]] auto get_omdb_service() const -> omdb::IService const& final { return *m_omdb; }

	klib::TypedLogger<Instance> m_log{};

	std::optional<kcurl::Curl> m_curl{};
	HttpGateway m_gateway{};
	std::optional<OmdbService> m_omdb{};
};
} // namespace
} // namespace detail

auto Instance::create(CreateInfo const& create_info, omdb::GetApiToken get_omdb_api_token) -> std::unique_ptr<Instance> {
	return std::make_unique<detail::Instance>(create_info, std::move(get_omdb_api_token));
}
} // namespace mediatool
