#pragma once
#include "command/command.hpp"
#include "klib/ptr.hpp"
#include "mediatool/omdb.hpp"
#include <string_view>

namespace mediatool::cli {
class Search : public Command {
  public:
	explicit Search(omdb::IService const& omdb_service) : m_omdb(&omdb_service) {}

  private:
	static constexpr auto name_v = std::string_view{"search"};

	[[nodiscard]] auto get_name() const -> std::string_view final { return name_v; }
	void populate_args() final;
	[[nodiscard]] auto execute() -> ExitCode final;

	klib::Ptr<omdb::IService const> m_omdb{};

	omdb::Query m_query{};
	std::string_view m_type{};
};
} // namespace mediatool::cli
