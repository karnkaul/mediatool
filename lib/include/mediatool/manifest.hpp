#pragma once
#include "mediatool/types.hpp"
#include <optional>
#include <variant>

namespace mediatool {
struct MovieManifest {
	Movie movie{};
	std::string title{};
	fs::path directory{};
};

struct EpisodeManifest {
	Episode episode;
	std::string title{};
	fs::path directory{};
};

struct SeasonManifest {
	Season season;
	std::string title{};
};

struct SeriesManifest {
	Series series;
	std::string title{};
};

using Manifest = std::variant<MovieManifest, EpisodeManifest, SeasonManifest, SeriesManifest>;

[[nodiscard]] auto build_manifest(fs::path const& path) -> std::optional<Manifest>;
} // namespace mediatool
