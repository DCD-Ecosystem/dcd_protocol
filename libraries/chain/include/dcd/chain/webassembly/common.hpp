#pragma once

#include <dcd/chain/wasm_interface.hpp>
#include <dcd/chain/wasm_dcd_constraints.hpp>
#include <dcd/vm/host_function.hpp>
#include <dcd/vm/argument_proxy.hpp>
#include <dcd/vm/span.hpp>
#include <dcd/vm/types.hpp>

using namespace fc;

namespace dcd { namespace chain { namespace webassembly {
   // forward declaration
   class interface;
}}} // ns dcd::chain::webassembly

// TODO need to fix up wasm_tests to not use this macro
#define DCD_INJECTED_MODULE_NAME "dcd_injection"

namespace dcd { namespace chain {

   class apply_context;
   class transaction_context;

   inline static constexpr auto dcd_injected_module_name = DCD_INJECTED_MODULE_NAME;

   template <typename T, std::size_t Extent = dcd::vm::dynamic_extent>
   using span = dcd::vm::span<T, Extent>;

   template <typename T, std::size_t Align = alignof(T)>
   using legacy_ptr = dcd::vm::argument_proxy<T*, Align>;

   template <typename T, std::size_t Align = alignof(T)>
   using legacy_span = dcd::vm::argument_proxy<dcd::vm::span<T>, Align>;

   struct null_terminated_ptr {
      null_terminated_ptr(const char* ptr) : ptr(ptr) {}
      const char* data() const { return ptr; }
      const char* ptr;
   };

   struct memcpy_params {
      void* dst;
      const void* src;
      vm::wasm_size_t size;
   };

   struct memcmp_params {
      const void* lhs;
      const void* rhs;
      vm::wasm_size_t size;
   };

   struct memset_params {
      const void* dst;
      const int32_t val;
      vm::wasm_size_t size;
   };

   // define the type converter for dcd
   struct type_converter : public dcd::vm::type_converter<webassembly::interface, dcd::vm::execution_interface> {
      using base_type = dcd::vm::type_converter<webassembly::interface, dcd::vm::execution_interface>;
      using base_type::type_converter;
      using base_type::from_wasm;
      using base_type::to_wasm;
      using base_type::as_value;
      using base_type::as_result;
      using base_type::elem_type;
      using base_type::get_host;

      DCD_VM_FROM_WASM(memcpy_params, (void* dst, const void* src, vm::wasm_size_t size)) {
         validate_pointer<char>(dst, size);
         validate_pointer<char>(src, size);
         validate_pointer<char>(dst, 1);
         return { dst, src, size };
      }

      DCD_VM_FROM_WASM(memcmp_params, (const void* lhs, const void* rhs, vm::wasm_size_t size)) {
         validate_pointer<char>(lhs, size);
         validate_pointer<char>(rhs, size);
         return { lhs, rhs, size };
      }

      DCD_VM_FROM_WASM(memset_params, (void* dst, int32_t val, vm::wasm_size_t size)) {
         validate_pointer<char>(dst, size);
         validate_pointer<char>(dst, 1);
         return { dst, val, size };
      }

      template <typename T>
      auto from_wasm(void* ptr) const
         -> std::enable_if_t< std::is_pointer_v<T>,
                              vm::argument_proxy<T> > {
         validate_pointer<std::remove_pointer_t<T>>(ptr, 1);
         return {ptr};
      }

      template <typename T>
      auto from_wasm(void* ptr, vm::tag<T> = {}) const
         -> std::enable_if_t< vm::is_argument_proxy_type_v<T> &&
                              std::is_pointer_v<typename T::proxy_type>, T> {
         if constexpr(T::is_legacy()) {
            DCD_ASSERT(ptr != this->get_interface().get_memory(), wasm_execution_error, "references cannot be created for null pointers");
         }
         this->template validate_pointer<typename T::pointee_type>(ptr, 1);
         return {ptr};
      }

      DCD_VM_FROM_WASM(null_terminated_ptr, (const void* ptr)) {
         validate_null_terminated_pointer(ptr);
         return {static_cast<const char*>(ptr)};
      }
      DCD_VM_FROM_WASM(name, (uint64_t e)) { return name{e}; }
      uint64_t to_wasm(name&& n) { return n.to_uint64_t(); }
      DCD_VM_FROM_WASM(float32_t, (float f)) { return ::to_softfloat32(f); }
      DCD_VM_FROM_WASM(float64_t, (double f)) { return ::to_softfloat64(f); }
   };

   using dcd_vm_host_functions_t = dcd::vm::registered_host_functions<webassembly::interface,
                                                                        dcd::vm::execution_interface,
                                                                        dcd::chain::type_converter>;
   using wasm_size_t = dcd::vm::wasm_size_t;

}} // dcd::chain
