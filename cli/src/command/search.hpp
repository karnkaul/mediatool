#pragma once
#include "command/command.hpp"
#include "mediatool/omdb.hpp"
#include <string_view>

namespace mediatool::cli {
class Search : public Command {
  public:
	static constexpr auto name_v = std::string_view{"search"};

	explicit Search();

  private:
	[[nodiscard]] auto get_name() const -> std::string_view final { return name_v; }
	[[nodiscard]] auto execute(Instance const& instance) -> int final;

	omdb::IService::Query m_query{};
	std::string_view m_media_type{};
};
} // namespace mediatool::cli
