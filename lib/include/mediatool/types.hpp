#pragma once
#include "klib/enum_name.hpp"
#include <compare>
#include <cstdint>
#include <filesystem>
#include <string>

namespace mediatool {
namespace fs = std::filesystem;

enum class MediaType : std::int8_t { Movie, Episode, Season, Series };
auto const media_type_name_map = klib::EnumNameMap<MediaType>{
	{MediaType::Movie, "movie"},
	{MediaType::Episode, "episode"},
	{MediaType::Season, "season"},
	{MediaType::Series, "series"},
};

class SeasonId {
  public:
	explicit SeasonId(int number);

	[[nodiscard]] auto get_number() const -> int { return m_number; }
	[[nodiscard]] auto as_string_view() const -> std::string_view { return m_str; }

	auto operator==(SeasonId const& rhs) const -> std::strong_ordering;

  private:
	int m_number{};
	std::string m_str{};
};

class EpisodeId {
  public:
	explicit EpisodeId(int season, int number);

	[[nodiscard]] auto get_number() const -> int { return m_number; }
	[[nodiscard]] auto get_season() const -> int { return m_season; }
	[[nodiscard]] auto as_string_view() const -> std::string_view { return m_str; }

	auto operator==(EpisodeId const& rhs) const -> std::strong_ordering;

  private:
	int m_season{};
	int m_number{};
	std::string m_str{};
};

struct Movie {
	fs::path path{};
	std::vector<fs::path> subtitles{};
};

struct Episode {
	EpisodeId id;
	fs::path path{};
	std::vector<fs::path> subtitles{};
};

struct Season {
	SeasonId id;
	fs::path path{};
	std::vector<Episode> episodes{};
};

struct Series {
	fs::path path{};
	std::vector<Season> seasons{};
};
} // namespace mediatool
