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
	explicit Instance(CreateInfo create_info) {
		create_curl(create_info.flags);
		create_omdb(std::move(create_info.omdb_token));
	}

  private:
	[[nodiscard]] auto get_omdb_service() const -> omdb::IService const& final { return *m_omdb; }

	void create_curl(InstanceCreateFlag const flags) {
		if ((flags & InstanceCreateFlag::ExternalCurl) == InstanceCreateFlag::None) { m_curl.emplace(); }
	}

	void create_omdb(std::string token) { m_omdb.emplace(m_gateway, std::move(token)); }

	klib::TypedLogger<Instance> m_log{};

	std::optional<kcurl::Curl> m_curl{};
	HttpGateway m_gateway{};
	std::optional<OmdbService> m_omdb{};
};
} // namespace
} // namespace detail

auto Instance::create(CreateInfo create_info) -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(std::move(create_info)); }
} // namespace mediatool
