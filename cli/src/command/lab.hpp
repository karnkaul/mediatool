#pragma once
#include "command/command.hpp"

namespace mediatool::cli {
class Lab : public Command {
  public:
	static constexpr auto name_v = std::string_view{"lab"};

	explicit Lab();

  private:
	[[nodiscard]] auto get_name() const -> std::string_view final { return name_v; }
	[[nodiscard]] auto execute(Instance const& instance) -> int final;

	std::string_view m_path{};
};
} // namespace mediatool::cli
