#pragma once

#include <b1/rodeos/callbacks/vm_types.hpp>
#include <dcd/name.hpp>
#include <dcd/stream.hpp>
#include <dcd/chain/exceptions.hpp>

namespace b1::rodeos {

struct action_state {
   dcd::name         receiver{};
   dcd::input_stream action_data{};
   std::vector<char>   action_return_value{};
};

template <typename Derived>
struct action_callbacks {
   Derived& derived() { return static_cast<Derived&>(*this); }

   int read_action_data(dcd::vm::span<char> data) {
      auto&  state = derived().get_state();
      size_t s     = state.action_data.end - state.action_data.pos;
      memcpy(data.data(), state.action_data.pos, std::min(data.size(), s));
      return s;
   }

   int action_data_size() {
      auto& state = derived().get_state();
      return state.action_data.end - state.action_data.pos;
   }

   uint64_t current_receiver() { return derived().get_state().receiver.value; }

   void set_action_return_value(dcd::vm::span<const char> packed_blob) {
      uint32_t max_action_return_value_size = 
         derived().get_state().shared->max_action_return_value_size;
      EOS_ASSERT(packed_blob.size() <= max_action_return_value_size,
                 dcd::chain::action_return_value_exception, 
                 "action return value size must be less than ${s} bytes", 
                 ("s", max_action_return_value_size));
      derived().get_state().action_return_value.assign(packed_blob.begin(), packed_blob.end());
   }

   template <typename Rft>
   static void register_callbacks() {
      // todo: preconditions
      Rft::template add<&Derived::read_action_data>("env", "read_action_data");
      Rft::template add<&Derived::action_data_size>("env", "action_data_size");
      Rft::template add<&Derived::current_receiver>("env", "current_receiver");
      Rft::template add<&Derived::set_action_return_value>("env", "set_action_return_value");
   }
}; // action_callbacks

} // namespace b1::rodeos
