#pragma once
#include "djson/json.hpp"
#include "mediatool/omdb.hpp"

namespace mediatool::detail {
void from_json(dj::Json const& json, omdb::Movie& out);
void from_json(dj::Json const& json, omdb::Episode& out);
void from_json(dj::Json const& json, omdb::Season& out);
void from_json(dj::Json const& json, omdb::Series& out);
} // namespace mediatool::detail
