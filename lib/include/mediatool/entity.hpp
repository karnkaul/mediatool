#pragma once
#include "mediatool/types.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace mediatool {
namespace fs = std::filesystem;

struct Entity {
	EntryType entry_type{};
	MediaType media_type{};
	fs::path path{};
	std::string title{};
};

[[nodiscard]] auto identify_entity(fs::path path) -> std::optional<Entity>;
} // namespace mediatool
