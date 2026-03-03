#pragma once
#include "klib/enum_name.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace mediatool::detail {
enum class TitleTokenType : std::int8_t { Word, BracketOpen, BracketClose };
inline static auto const title_token_name = klib::EnumNameMap<TitleTokenType>{
	{TitleTokenType::Word, "word"},
	{TitleTokenType::BracketOpen, "bracket_open"},
	{TitleTokenType::BracketClose, "bracket_close"},
};

struct TitleToken {
	std::string_view lexeme{};
	TitleTokenType type{};
};

class TitleScanner {
  public:
	explicit constexpr TitleScanner(std::string_view const title) : m_remain(title) {}

	[[nodiscard]] constexpr auto next(TitleToken& out) -> bool {
		skip_whitespace();
		if (m_remain.empty()) { return false; }
		if (scan_bracket(out)) { return true; }
		out = scan_word();
		return true;
	}

  private:
	using Type = TitleTokenType;

	static constexpr auto is_whitespace(char const ch) -> bool {
		constexpr auto whitespace_v = std::array{' ', '\t', '.'};
		return std::ranges::find(whitespace_v, ch) != whitespace_v.end();
	}

	static constexpr auto is_open_bracket(char const ch) -> bool {
		constexpr auto open_bracket_v = std::array{'(', '['};
		return std::ranges::find(open_bracket_v, ch) != open_bracket_v.end();
	}

	static constexpr auto is_close_bracket(char const ch) -> bool {
		constexpr auto close_bracket_v = std::array{')', ']'};
		return std::ranges::find(close_bracket_v, ch) != close_bracket_v.end();
	}

	static constexpr auto is_bracket(char const ch) -> bool { return is_open_bracket(ch) || is_close_bracket(ch); }

	constexpr void skip_whitespace() {
		while (!m_remain.empty() && is_whitespace(m_remain.front())) { m_remain.remove_prefix(1); }
	}

	constexpr auto to_token(Type const type, std::size_t const length) -> TitleToken {
		auto const ret = TitleToken{.lexeme = m_remain.substr(0, length), .type = type};
		m_remain.remove_prefix(length);
		return ret;
	}

	constexpr auto scan_bracket(TitleToken& out) -> bool {
		auto const ch = m_remain.front();
		if (is_open_bracket(ch)) {
			out = to_token(Type::BracketOpen, 1);
			return true;
		}
		if (is_close_bracket(ch)) {
			out = to_token(Type::BracketClose, 1);
			return true;
		}
		return false;
	}

	constexpr auto scan_word() -> TitleToken {
		auto length = 1uz;
		for (; length < m_remain.size() && !is_whitespace(m_remain.at(length)); ++length) {
			char const ch = m_remain.at(length);
			if (is_whitespace(ch) || is_bracket(ch)) { break; }
		}
		return to_token(Type::Word, length);
	}

	std::string_view m_remain{};
};

class TitleParser {
  public:
	[[nodiscard]] auto parse(std::string_view stem) -> std::string;

  private:
	using Type = TitleTokenType;

	auto parse(TitleToken const& token) -> bool;

	std::string m_title{};
	int m_bracket_depth{0};
};
} // namespace mediatool::detail
