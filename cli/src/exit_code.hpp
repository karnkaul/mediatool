#pragma once
#include <cstdint>
#include <cstdlib>

namespace mediatool {
enum class ExitCode : std::int8_t {
	Success = EXIT_SUCCESS,
	Failure = EXIT_FAILURE,

	InvalidPath = 10,
	ManifestFailure = 11,
	OmdbServiceFailure = 15,
};
} // namespace mediatool
