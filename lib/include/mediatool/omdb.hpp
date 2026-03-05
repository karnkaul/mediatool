#pragma once
#include "djson/json.hpp"
#include "kcurl/http.hpp"
#include "klib/base_types.hpp"
#include "klib/enum_name.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace mediatool::omdb {
enum class Type : std::int8_t { Movie, Series, Episode, COUNT_ };
auto const type_map = klib::EnumNameMap<Type>{
	{Type::Movie, "movie"},
	{Type::Series, "series"},
	{Type::Episode, "episode"},
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

enum class Curl : std::int8_t {
	/// \brief IService owns curl initialization/shutdown.
	Internal,
	/// \brief IService does not own curl initialization/shutdown.
	External
};

using GetApiToken = std::move_only_function<std::string_view()>;

struct Query {
	std::string_view title{};
	int season{};
	int episode{};
};

class IService : public klib::Polymorphic {
  public:
	[[nodiscard]] static auto create(GetApiToken get_api_token, Curl curl = Curl::Internal) -> std::unique_ptr<IService>;

	[[nodiscard]] virtual auto search(Query const& query, std::optional<Type> type = {}) const -> omdb::Result<Payload> = 0;
};
} // namespace mediatool::omdb
