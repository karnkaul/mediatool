#pragma once
#include "command/command.hpp"

namespace mediatool::cli {
class Identify : public Command {
  public:
	static constexpr auto name_v = std::string_view{"identify"};

	explicit Identify();

  private:
	[[nodiscard]] auto get_name() const -> std::string_view final { return name_v; }
	[[nodiscard]] auto execute(Instance const& instance) -> int final;

	std::string_view m_directory{};
};
} // namespace mediatool::cli
