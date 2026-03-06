#pragma once
#include "exit_code.hpp"
#include "klib/args/arg.hpp"
#include "klib/base_types.hpp"
#include <span>
#include <string_view>
#include <vector>

namespace mediatool::cli {

class Command : public klib::Polymorphic, public klib::Pinned {
  public:
	[[nodiscard]] virtual auto get_name() const -> std::string_view = 0;
	virtual void populate_args() {}

	[[nodiscard]] virtual auto execute() -> ExitCode = 0;

	[[nodiscard]] auto get_args() const -> std::span<klib::args::Arg const> { return m_args; }

  protected:
	std::vector<klib::args::Arg> m_args{};
};
} // namespace mediatool::cli
