#pragma once
#include "klib/args/arg.hpp"
#include "klib/base_types.hpp"
#include "klib/ptr.hpp"
#include "mediatool/omdb.hpp"
#include <string_view>
#include <vector>

namespace mediatool::cli {
class Command : public klib::Polymorphic, public klib::Pinned {
  public:
	[[nodiscard]] virtual auto get_name() const -> std::string_view = 0;
	[[nodiscard]] virtual auto get_args() -> std::vector<klib::args::Arg> { return {}; }

	[[nodiscard]] virtual auto execute() -> int = 0;

  protected:
	/// \brief Is non-null inside execute().
	[[nodiscard]] auto get_omdb_service() const -> klib::Ptr<omdb::IService const> { return m_omdb; }

  private:
	klib::Ptr<omdb::IService const> m_omdb{};

	friend class App;
};
} // namespace mediatool::cli
