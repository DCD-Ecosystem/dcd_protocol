#pragma once

#include <b1/roddcd/callbacks/basic.hpp>
#include <b1/roddcd/callbacks/chaindb.hpp>
#include <b1/roddcd/callbacks/compiler_builtins.hpp>
#include <b1/roddcd/callbacks/console.hpp>
#include <b1/roddcd/callbacks/filter.hpp>
#include <b1/roddcd/callbacks/memory.hpp>
#include <b1/roddcd/callbacks/unimplemented.hpp>
#include <b1/roddcd/callbacks/unimplemented_filter.hpp>

// todo: configure limits
// todo: timeout
namespace b1::roddcd::filter {

struct callbacks;
using rhf_t     = registered_host_functions<callbacks>;
using backend_t = dcd::vm::backend<rhf_t, dcd::vm::jit>;

struct filter_state : b1::roddcd::data_state<backend_t>, b1::roddcd::console_state, b1::roddcd::filter_callback_state {
   dcd::vm::wasm_allocator wa = {};
};

struct callbacks : b1::roddcd::chaindb_callbacks<callbacks>,
                   b1::roddcd::compiler_builtins_callbacks<callbacks>,
                   b1::roddcd::console_callbacks<callbacks>,
                   b1::roddcd::context_free_system_callbacks<callbacks>,
                   b1::roddcd::data_callbacks<callbacks>,
                   b1::roddcd::db_callbacks<callbacks>,
                   b1::roddcd::filter_callbacks<callbacks>,
                   b1::roddcd::memory_callbacks<callbacks>,
                   b1::roddcd::unimplemented_callbacks<callbacks>,
                   b1::roddcd::unimplemented_filter_callbacks<callbacks> {
   filter::filter_state&      filter_state;
   b1::roddcd::chaindb_state& chaindb_state;
   b1::roddcd::db_view_state& db_view_state;

   callbacks(filter::filter_state& filter_state, b1::roddcd::chaindb_state& chaindb_state,
             b1::roddcd::db_view_state& db_view_state)
       : filter_state{ filter_state }, chaindb_state{ chaindb_state }, db_view_state{ db_view_state } {}

   auto& get_state() { return filter_state; }
   auto& get_filter_callback_state() { return filter_state; }
   auto& get_chaindb_state() { return chaindb_state; }
   auto& get_db_view_state() { return db_view_state; }
};

inline void register_callbacks() {
   b1::roddcd::chaindb_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::compiler_builtins_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::console_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::context_free_system_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::data_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::db_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::filter_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::memory_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::unimplemented_callbacks<callbacks>::register_callbacks<rhf_t>();
   b1::roddcd::unimplemented_filter_callbacks<callbacks>::register_callbacks<rhf_t>();
}

} // namespace b1::roddcd::filter
