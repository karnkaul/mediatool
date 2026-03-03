#pragma once
#include "klib/base_types.hpp"
#include <string_view>

namespace mediatool {
class IApiTokenProvider : public klib::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_api_token() -> std::string_view = 0;
};
} // namespace mediatool
