#pragma once
#include "mediatool/types.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <variant>

namespace mediatool {
namespace fs = std::filesystem;

struct MovieDirectory {
	fs::path directory{};
	std::string title{};
};

struct EpisodeDirectory {
	std::optional<EpisodeId> id{};
	fs::path directory{};
	std::string title{};
};

struct SeasonDirectory {
	std::optional<SeasonId> id{};
	fs::path directory{};
	std::string title{};
};

struct SeriesDirectory {
	fs::path directory{};
	std::string title{};
};

using MediaDirectory = std::variant<MovieDirectory, EpisodeDirectory, SeasonDirectory, SeriesDirectory>;
} // namespace mediatool
