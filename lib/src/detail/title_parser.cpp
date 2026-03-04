#include "detail/title_parser.hpp"
#include "mediatool/panic.hpp"
#include <regex>
#include <string_view>

namespace mediatool::detail {
namespace {
[[nodiscard]] auto is_match(std::regex const& regex, std::string_view const word) -> bool {
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	return std::regex_match(word.data(), word.data() + word.size(), regex);
}

[[nodiscard]] auto is_year(std::string_view const word) -> bool {
	static auto const s_regex = std::regex{R"([1-3][0-9]{3})"};
	return is_match(s_regex, word);
}

[[nodiscard]] auto is_season_id(std::string_view const word) -> bool {
	static auto const s_regex = std::regex{R"(S[0-9]{2})"};
	return word == "Season" || is_match(s_regex, word);
}

[[nodiscard]] auto is_episode_id(std::string_view const word) -> bool {
	static auto const s_regex = std::regex{R"(S[0-9]{2}E[0-9]{2})"};
	return is_match(s_regex, word);
}

[[nodiscard]] auto is_resolution(std::string_view const word) -> bool {
	static auto const s_regex = std::regex{R"([1-9][0-9]{2}[0-9]?p)"};
	return is_match(s_regex, word);
}
} // namespace

auto TitleParser::parse(std::string_view const stem) -> std::string {
	m_title.clear();
	m_bracket_depth = 0;

	auto scanner = TitleScanner{stem};
	auto token = TitleToken{};
	while (scanner.next(token)) {
		if (!parse(token)) { break; }
	}
	return std::move(m_title);
}

auto TitleParser::parse(TitleToken const& token) -> bool {
	switch (token.type) {
	case Type::BracketOpen: ++m_bracket_depth; return true;
	case Type::BracketClose: m_bracket_depth = std::max(m_bracket_depth - 1, 0); return true;
	case Type::Word: break;
	default: throw Panic{"internal error: unexpected TitleTokenType"};
	}

	if (m_bracket_depth > 0) { return true; }
	if (m_skip_next) {
		m_skip_next = false;
		return true;
	}

	auto const word = token.lexeme;
	if (word == "-") { return true; }

	if (is_season_id(word)) {
		if (m_title.empty()) {
			m_skip_next = true; // Season XX, skip XX
			return true;
		}
		return false;
	}

	if (is_year(word) || is_episode_id(word) || is_resolution(word)) { return m_title.empty(); }

	if (!m_title.empty()) { m_title.push_back(' '); }
	m_title += word;
	return true;
}
} // namespace mediatool::detail
