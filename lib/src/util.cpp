#include "mediatool/util.hpp"
#include "detail/title_parser.hpp"
#include "klib/assert.hpp"
#include "mediatool/media_directory.hpp"
#include "mediatool/types.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <optional>
#include <regex>
#include <string_view>

namespace mediatool {
namespace util {
namespace {
using namespace std::string_view_literals;

constexpr auto video_extensions_v = std::array{
	".mp4"sv, ".mkv"sv, ".avi"sv, ".m4v"sv, ".webm"sv,
};

constexpr auto is_video_file(std::string_view const extension) { return std::ranges::find(video_extensions_v, extension) != video_extensions_v.end(); }

[[nodiscard]] auto is_season_dir(fs::path const& path) {
	static auto const s_regex_1 = std::regex{R"(.*Season.[0-9]{2}.*)"};
	static auto const s_regex_2 = std::regex{R"(.*S[0-9]{2}.*)"};
	auto const filename = path.stem().string();
	return std::regex_match(filename, s_regex_1) || std::regex_match(filename, s_regex_2);
}

[[nodiscard]] auto is_episode(fs::path const& path) {
	static auto const s_regex = std::regex{R"(.*S[0-9]{2}E[0-9]{2}.*)"};
	return std::regex_match(path.string(), s_regex);
}

[[nodiscard]] auto identify_directory_type(fs::path const& path) -> std::optional<MediaType> {
	KLIB_ASSERT(fs::is_directory(path));

	auto const str = path.string();
	if (is_season_dir(path)) { return MediaType::Season; }

	auto has_video_file = false;
	for (auto const& it : fs::directory_iterator{path}) {
		if (it.is_directory() && is_season_dir(it.path())) { return MediaType::Series; }

		if (!it.is_regular_file()) { continue; }
		auto const extension = it.path().extension().string();
		if (!is_video_file(extension)) { continue; }

		has_video_file = true;
		if (is_episode(it.path())) { return MediaType::Season; }
	}

	if (has_video_file) { return MediaType::Movie; }

	return {};
}

[[nodiscard]] auto identify_file_type(fs::path const& path) -> std::optional<MediaType> {
	KLIB_ASSERT(fs::is_regular_file(path));
	if (!is_video_file(path.extension().string())) { return {}; }
	if (is_episode(path)) { return MediaType::Episode; }
	return MediaType::Movie;
}

class MediaDirectoryIdentifier {
  public:
	[[nodiscard]] auto operator()(fs::path path) -> std::optional<MediaDirectory> {
		if (!fs::is_directory(path)) { return {}; }
		return identify_media_type(path).and_then([&](MediaType const mt) { return handle_directory(std::move(path), mt); });
	}

  private:
	[[nodiscard]] static auto handle_directory(fs::path path, MediaType const media_type) -> std::optional<MediaDirectory> {
		auto title = identify_title(path);
		switch (media_type) {
		case MediaType::Movie: return MovieDirectory{.directory = std::move(path), .title = std::move(title)};
		case MediaType::Episode: {
			auto title = identify_title(path);
			auto id = extract_episode_id(path.stem().string());
			return EpisodeDirectory{.id = std::move(id), .directory = std::move(path), .title = std::move(title)};
		}
		case MediaType::Season: {
			auto title = identify_title(path);
			auto id = extract_season_id(path.stem().string());
			return SeasonDirectory{.id = std::move(id), .directory = std::move(path), .title = std::move(title)};
		}
		case MediaType::Series: return SeriesDirectory{.directory = std::move(path), .title = std::move(title)};

		default: break;
		}

		return {};
	}
};
} // namespace
} // namespace util

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

auto util::identify_media_type(fs::path const& path) -> std::optional<MediaType> {
	if (fs::is_directory(path)) { return identify_directory_type(path); }
	if (fs::is_regular_file(path)) { return identify_file_type(path); }
	return {};
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

auto util::identify_media_directory(fs::path const& path) -> std::optional<MediaDirectory> { return MediaDirectoryIdentifier{}(path); }
} // namespace mediatool
