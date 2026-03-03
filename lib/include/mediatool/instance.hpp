#pragma once
#include "klib/base_types.hpp"
#include "klib/enum_ops.hpp"
#include "mediatool/omdb.hpp"
#include <cstdint>
#include <memory>

namespace mediatool {
enum class InstanceCreateFlag : std::uint8_t {
	None = 0,
	ExternalCurl = 1 << 0,
};
[[nodiscard]] constexpr auto enable_enum_bitops(InstanceCreateFlag /*unused*/) { return true; }

struct InstanceCreateInfo {
	using Flag = InstanceCreateFlag;

	Flag flags{};
};

class Instance : public klib::Polymorphic {
  public:
	using CreateInfo = InstanceCreateInfo;

	[[nodiscard]] static auto create(CreateInfo const& create_info, omdb::GetApiToken get_omdb_api_token) -> std::unique_ptr<Instance>;

	[[nodiscard]] virtual auto get_omdb_service() const -> omdb::IService const& = 0;
};
} // namespace mediatool
