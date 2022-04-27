#pragma once

#include <dcd/state_history/types.hpp>
#include <dcd/chain/combined_database.hpp>

namespace dcd {
namespace state_history {

std::vector<table_delta> create_deltas(const chain::combined_database& db, bool full_snapshot);

} // namespace state_history
} // namespace dcd
