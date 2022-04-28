#pragma once

#include <dcd/name.hpp>

namespace b1::roddcd {

// kv database which stores roddcd state, including a mirror of dcdnode state
inline constexpr dcd::name state_database{ "dcd.state" };

// account within state_database which stores state
inline constexpr dcd::name state_account{ "state" };

} // namespace b1::roddcd
