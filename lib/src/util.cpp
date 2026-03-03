#include "mediatool/util.hpp"
#include "detail/title_parser.hpp"
#include "mediatool/types.hpp"
#include <optional>
#include <regex>
#include <string_view>

namespace mediatool {
auto util::get_env_var(klib::CString const key) -> klib::CString {
	if (key.as_view().empty()) { return {}; }
	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	return std::getenv(key.c_str());
}

auto util::to_int(std::string_view const text, int const fallback) -> int {
	if (text.empty()) { return fallback; }
	auto ret = int{};
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	auto const* end = text.data() + text.size();
	auto const [_, ec] = std::from_chars(text.data(), end, ret);
	if (ec != std::errc{}) { return fallback; }
	return ret;
}

auto util::identify_title(fs::path const& path) -> std::string { return detail::TitleParser{}.parse(path.stem().string()); }

auto util::extract_season_id(std::string const& name) -> std::optional<SeasonId> {
	if (name.empty()) { return {}; }

	static auto const s_regex_1 = std::regex{R"(Season.[0-9]{2})"};
	static auto const s_regex_2 = std::regex{R"(S[0-9]{2})"};
	auto matches = std::smatch{};
	if (!std::regex_search(name, matches, s_regex_1) && !std::regex_search(name, matches, s_regex_2)) { return {}; }
	auto const str = std::string{matches[0]};
	auto const number = to_int(std::string_view{str}.substr(str.size() - 2));
	if (number <= 0) { return {}; }
	return SeasonId{number};
}

auto util::extract_episode_id(std::string const& name) -> std::optional<EpisodeId> {
	if (name.empty()) { return {}; }

	static auto const s_regex = std::regex{R"(S[0-9]{2}E[0-9]{2})"};
	auto matches = std::smatch{};
	if (!std::regex_search(name, matches, s_regex)) { return {}; }
	auto const str = std::string{matches[0]};
	auto const number = to_int(std::string_view{str}.substr(str.size() - 2));
	auto const season = to_int(std::string_view{str}.substr(1, 2));
	if (number <= 0 || season <= 0) { return {}; }
	return EpisodeId{season, number};
}
} // namespace mediatool
