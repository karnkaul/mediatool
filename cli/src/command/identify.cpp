#include "command/identify.hpp"
#include "klib/args/arg.hpp"
#include "log.hpp"
#include "mediatool/manifest.hpp"
#include "serialize.hpp"
#include <filesystem>
#include <print>

namespace mediatool::cli {
void Identify::populate_args() {
	m_args = {
		klib::args::positional_required(m_path, "path"),
	};
}

auto Identify::execute() -> ExitCode {
	auto const path = fs::path{m_path};
	if (!fs::exists(path)) {
		log.error("nonexistent path: '{}'", m_path);
		return ExitCode::InvalidPath;
	}

	auto const manifest = build_manifest(path);
	if (!manifest) {
		log.error("failed to build manifest for: '{}'", m_path);
		return ExitCode::ManifestFailure;
	}

	std::visit([](auto const& manifest) { std::println("{}", serialize(manifest)); }, *manifest);

	return ExitCode::Success;
}
} // namespace mediatool::cli
