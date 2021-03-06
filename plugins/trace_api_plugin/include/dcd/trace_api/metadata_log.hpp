#pragma once
#include <fc/variant.hpp>
#include <dcd/trace_api/trace.hpp>
#include <dcd/chain/abi_def.hpp>
#include <dcd/chain/protocol_feature_activation.hpp>

namespace dcd { namespace trace_api {
   struct block_entry_v0 {
      chain::block_id_type   id;
      uint32_t               number;
      uint64_t               offset;
   };

   struct lib_entry_v0 {
      uint32_t               lib;
   };

   using metadata_log_entry = std::variant<
      block_entry_v0,
      lib_entry_v0
   >;

}}

FC_REFLECT(dcd::trace_api::block_entry_v0, (id)(number)(offset));
FC_REFLECT(dcd::trace_api::lib_entry_v0, (lib));
