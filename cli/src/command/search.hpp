#pragma once
#include "command/command.hpp"
#include <string_view>

namespace mediatool::cli {
class Search : public Command {
  public:
	static constexpr auto name_v = std::string_view{"search"};

	explicit Search();

  private:
	[[nodiscard]] auto get_name() const -> std::string_view final { return name_v; }
	[[nodiscard]] auto execute(Instance const& instance) -> int final;

	[[nodiscard]] auto search_episode(omdb::IService const& omdb) const -> int;
	[[nodiscard]] auto search_season(omdb::IService const& omdb) const -> int;
	[[nodiscard]] auto search_series(omdb::IService const& omdb) const -> int;
	[[nodiscard]] auto search_movie(omdb::IService const& omdb) const -> int;
	[[nodiscard]] auto search_generic(omdb::IService const& omdb) const -> int;

	std::string_view m_title{};
	std::string_view m_media_type{};
	int m_season{};
	int m_episode{};
};
} // namespace mediatool::cli
