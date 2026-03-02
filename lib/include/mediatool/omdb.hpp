#pragma once
#include "djson/json.hpp"
#include "kcurl/http.hpp"
#include "klib/base_types.hpp"
#include "klib/enum_name.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace mediatool::omdb {
enum class MediaType : std::int8_t { Movie, Series, Episode, COUNT_ };

auto const media_type_map = klib::EnumNameMap<MediaType>{
	{MediaType::Movie, "movie"},
	{MediaType::Series, "series"},
	{MediaType::Episode, "episode"},
};

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

using Payload = std::variant<Movie, Episode, Season, Series, dj::Json>;

template <typename Type>
using Result = kcurl::http::Result<Type>;

class IService : public klib::Polymorphic {
  public:
	struct Query {
		std::string_view title{};
		int season{};
		int episode{};
	};

	virtual void set_api_token(std::string token) = 0;
	[[nodiscard]] virtual auto get_api_token() const -> std::string_view = 0;

	[[nodiscard]] virtual auto search(Query const& query, std::optional<MediaType> type = {}) const -> omdb::Result<Payload> = 0;
};
} // namespace mediatool::omdb
