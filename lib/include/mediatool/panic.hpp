#pragma once
#include <stdexcept>

namespace mediatool {
struct Panic : std::runtime_error {
	using std::runtime_error::runtime_error;
};
} // namespace mediatool
