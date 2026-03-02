#include "mediatool/types.hpp"
#include <format>

namespace mediatool {
SeasonId::SeasonId(int const number) : m_number(number), m_str(std::format("Season {:02}", m_number)) {}

auto SeasonId::operator==(SeasonId const& rhs) const -> std::strong_ordering { return get_number() <=> rhs.get_number(); }

EpisodeId::EpisodeId(int const season, int const number) : m_season(season), m_number(number), m_str(std::format("S{:02}E{:02}", m_season, m_number)) {}

auto EpisodeId::operator==(EpisodeId const& rhs) const -> std::strong_ordering {
	if (auto const result = get_season() <=> rhs.get_season(); result != 0) { return result; }
	return get_number() <=> rhs.get_number();
}
} // namespace mediatool
