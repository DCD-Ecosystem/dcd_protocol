#pragma once
#include <fc/variant.hpp>
#include <dcd/trace_api/trace.hpp>
#include <dcd/chain/abi_def.hpp>
#include <dcd/chain/protocol_feature_activation.hpp>

namespace dcd { namespace trace_api {

   using data_log_entry = std::variant<
      block_trace_v0,
      block_trace_v1,
      block_trace_v2
   >;

}}
