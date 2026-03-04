#include "detail/identity.hpp"
#include "klib/assert.hpp"
#include "mediatool/types.hpp"
#include "mediatool/util.hpp"
#include <filesystem>
#include <optional>

namespace mediatool {
namespace detail {
namespace {
struct Types {
	EntryType entry_type{};
	MediaType media_type{};
};

[[nodiscard]] auto identify_entry_type(fs::path const& path) -> std::optional<EntryType> {
	if (fs::is_regular_file(path)) { return EntryType::File; }
	if (fs::is_directory(path)) { return EntryType::Directory; }
	return {};
}

[[nodiscard]] auto identify_directory_type(fs::path const& path) -> std::optional<MediaType> {
	KLIB_ASSERT(fs::is_directory(path));

	auto const str = path.string();
	if (util::is_season_directory(path)) { return MediaType::Season; }

	auto has_video_file = false;
	for (auto const& it : fs::directory_iterator{path}) {
		if (it.is_directory() && util::is_season_directory(it.path())) { return MediaType::Series; }

		if (!it.is_regular_file()) { continue; }
		auto const extension = it.path().extension().string();
		if (!util::is_video_file(extension)) { continue; }

		has_video_file = true;
		if (util::is_episode(it.path())) { return MediaType::Season; }
	}

	if (has_video_file) { return MediaType::Movie; }

	return {};
}

[[nodiscard]] auto identify_file_type(fs::path const& path) -> std::optional<MediaType> {
	KLIB_ASSERT(fs::is_regular_file(path));
	if (!util::is_video_file(path.extension().string())) { return {}; }
	if (util::is_episode(path)) { return MediaType::Episode; }
	return MediaType::Movie;
}

[[nodiscard]] auto identify_types(fs::path const& path) -> std::optional<Types> {
	auto const entry_type = identify_entry_type(path);
	if (!entry_type) { return {}; }

	auto const media_type = *entry_type == EntryType::Directory ? identify_directory_type(path) : identify_file_type(path);
	if (!media_type) { return {}; }

	return Types{.entry_type = *entry_type, .media_type = *media_type};
}

[[nodiscard]] auto to_identity(fs::path path, Types const types) {
	auto title = util::identify_title(path);
	return Identity{
		.entry_type = types.entry_type,
		.media_type = types.media_type,
		.path = std::move(path),
		.title = std::move(title),
	};
}
} // namespace
} // namespace detail

auto detail::identify(fs::path path) -> std::optional<Identity> {
	return identify_types(path).transform([&](Types const types) { return to_identity(std::move(path), types); });
}
} // namespace mediatool
