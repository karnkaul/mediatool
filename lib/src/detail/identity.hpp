#pragma once
#include "mediatool/types.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace mediatool::detail {
namespace fs = std::filesystem;

enum class EntryType : std::int8_t { Directory, File };

struct Identity {
	EntryType entry_type{};
	MediaType media_type{};
	fs::path path{};
	std::string title{};
};

[[nodiscard]] auto identify(fs::path path) -> std::optional<Identity>;
} // namespace mediatool::detail
