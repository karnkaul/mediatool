#pragma once
#include "mediatool/types.hpp"
#include <optional>
#include <variant>

namespace mediatool {
struct MovieManifest {
	void serialize_to(std::string& out) const;

	Movie movie{};
	std::string title{};
	fs::path directory{};
};

struct EpisodeManifest {
	void serialize_to(std::string& out) const;

	Episode episode;
	std::string title{};
	fs::path directory{};
};

struct SeasonManifest {
	void serialize_to(std::string& out) const;

	Season season;
	std::string title{};
};

struct SeriesManifest {
	void serialize_to(std::string& out) const;

	Series series;
	std::string title{};
};

using Manifest = std::variant<MovieManifest, EpisodeManifest, SeasonManifest, SeriesManifest>;

[[nodiscard]] auto build_manifest(fs::path const& path) -> std::optional<Manifest>;
} // namespace mediatool
