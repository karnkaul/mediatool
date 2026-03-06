#pragma once
#include "djson/json.hpp"
#include <string>

namespace mediatool::cli {
template <typename T>
[[nodiscard]] auto serialize(T const& t) -> std::string {
	auto ret = std::string{};
	t.serialize_to(ret);
	return ret;
}

[[nodiscard]] inline auto serialize(dj::Json const& json) -> std::string { return json.serialize(); }
} // namespace mediatool::cli
