#include "mediatool/media_file.hpp"
#include "mediatool/util.hpp"
#include <filesystem>

namespace mediatool {
namespace {
class Collector {
  public:
	[[nodiscard]] auto collect(fs::path const& path) -> std::vector<MediaFile> {
		if (fs::is_regular_file(path)) {
			collect_file(path);
		} else if (fs::is_directory(path)) {
			collect_directory(path);
		}
		return std::move(m_files);
	}

  private:
	using Type = MediaFile::Type;

	[[nodiscard]] static auto to_media_file(Type const type, fs::path path) -> MediaFile {
		auto const size = std::uint64_t(fs::file_size(path));
		return MediaFile{.type = type, .path = std::move(path), .size = size};
	}

	void collect_file(fs::path path) {
		auto const extension = path.extension().string();
		if (util::is_video_file(extension)) {
			m_files.push_back(to_media_file(Type::Video, std::move(path)));
		} else if (util::is_subtitle_file(extension)) {
			m_files.push_back(to_media_file(Type::Subtitle, std::move(path)));
		}
	}

	void collect_directory(fs::path const& path) {
		for (auto const& it : fs::directory_iterator{path}) {
			if (it.is_directory()) {
				auto const name = it.path().filename().string();
				if (name == "Subtitles" || name == "Subs") { collect_subtitles(it.path()); }

				continue;
			}

			if (!it.is_regular_file()) { continue; }
			collect_file(it.path());
		}
	}

	void collect_subtitles(fs::path const& path) {
		for (auto const& it : fs::recursive_directory_iterator{path}) {
			if (!it.is_regular_file()) { continue; }
			if (!util::is_subtitle_file(it.path().extension().string())) { continue; }
			m_files.push_back(to_media_file(Type::Subtitle, it.path()));
		}
	}

	std::vector<MediaFile> m_files{};
};
} // namespace
} // namespace mediatool

auto mediatool::collect_media_files(fs::path const& path) -> std::vector<MediaFile> { return Collector{}.collect(path); }
