#pragma once
#include "ship_protocol.hpp"

namespace chain_types {
using namespace dcd::ship_protocol;

struct block_info {
   uint32_t               block_num = {};
   dcd::checksum256     block_id  = {};
   dcd::block_timestamp timestamp;
};

DCD_REFLECT(block_info, block_num, block_id, timestamp);
}; // namespace chain_types