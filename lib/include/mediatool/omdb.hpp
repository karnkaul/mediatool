#pragma once
#include "djson/json.hpp"
#include "kcurl/http.hpp"
#include "klib/base_types.hpp"
#include "klib/enum_array.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace mediatool::omdb {
enum class MediaType : std::int8_t { None, Movie, Series, Episode, COUNT_ };

constexpr auto media_type_str_v = klib::EnumArray<MediaType, std::string_view>{"none", "movie", "series", "episode"};

struct Movie {
	std::string title{};
	int year{};
	std::string imdb_id{};
	std::string plot{};
};

struct Episode {
	int number{};
	std::string title{};
	std::string imdb_id{};
	std::string plot{};
};

struct Season {
	int number{};
	std::string title{};
	std::vector<Episode> episodes{};
};

struct Series {
	std::string title{};
	int year{};
	std::string imdb_id{};
	std::string plot{};
	int total_seasons{};
};

template <typename Type>
using Result = kcurl::http::Result<Type>;

class IService : public klib::Polymorphic {
  public:
	virtual void set_api_token(std::string token) = 0;
	[[nodiscard]] virtual auto get_api_token() const -> std::string_view = 0;

	[[nodiscard]] virtual auto search_movie(std::string_view title) const -> Result<Movie> = 0;
	[[nodiscard]] virtual auto search_series(std::string_view title) const -> Result<Series> = 0;
	[[nodiscard]] virtual auto search_season(std::string_view title, int season) const -> Result<Season> = 0;
	[[nodiscard]] virtual auto search_episode(std::string_view title, int season, int episode) const -> Result<Episode> = 0;

	[[nodiscard]] virtual auto search_generic(std::string_view title) const -> omdb::Result<dj::Json> = 0;
};
} // namespace mediatool::omdb
