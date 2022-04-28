#pragma once

#include <b1/roddcd/callbacks/vm_types.hpp>
#include <dcd/stream.hpp>

namespace b1::roddcd {

struct assert_exception : std::exception {
   std::string msg;

   assert_exception(std::string&& msg) : msg(std::move(msg)) {}

   const char* what() const noexcept override { return msg.c_str(); }
};

template <typename Derived>
struct context_free_system_callbacks {
   Derived& derived() { return static_cast<Derived&>(*this); }

   void abort() { throw std::runtime_error("called abort"); }

   void dcd_assert(bool condition, null_terminated_ptr msg) {
      if (!condition)
         throw assert_exception(std::string(msg.data(), msg.size()));
   }

   void dcd_assert_message(bool condition, legacy_span<const char> msg) {
      if (!condition)
         throw assert_exception(std::string(msg.data(), msg.size()));
   }

   template <typename Rft>
   static void register_callbacks() {
      // todo: preconditions
      Rft::template add<&Derived::abort>("env", "abort");
      Rft::template add<&Derived::dcd_assert>("env", "dcd_assert");
      Rft::template add<&Derived::dcd_assert_message>("env", "dcd_assert_message");
   }
};

template <typename Backend>
struct data_state {
   dcd::input_stream input_data;
   std::vector<char>   output_data;
};

template <typename Derived>
struct data_callbacks {
   Derived& derived() { return static_cast<Derived&>(*this); }

   uint32_t get_input_data(dcd::vm::span<char> dest) {
      auto& input_data = derived().get_state().input_data;
      memcpy(dest.data(), input_data.pos, std::min(dest.size(), size_t(input_data.end - input_data.pos)));
      return input_data.end - input_data.pos;
   }

   void set_output_data(dcd::vm::span<const char> data) {
      auto& output_data = derived().get_state().output_data;
      output_data.clear();
      output_data.insert(output_data.end(), data.begin(), data.end());
   }

   template <typename Rft>
   static void register_callbacks() {
      // todo: preconditions
      Rft::template add<&Derived::get_input_data>("env", "get_input_data");
      Rft::template add<&Derived::set_output_data>("env", "set_output_data");
   }
};

} // namespace b1::roddcd
