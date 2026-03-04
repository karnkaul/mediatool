#pragma once
#include "klib/enum_name.hpp"
#include <filesystem>
#include <vector>

namespace mediatool::detail {
namespace fs = std::filesystem;

struct MediaFile {
	enum class Type : std::int8_t { Video, Subtitle };
	inline static auto const type_name_map = klib::EnumNameMap<Type>{
		{Type::Video, "video"},
		{Type::Subtitle, "subtitle"},
	};

	Type type{};
	fs::path path{};
	std::uint64_t size{};
};

[[nodiscard]] auto collect_media_files(fs::path const& path) -> std::vector<MediaFile>;
} // namespace mediatool::detail
