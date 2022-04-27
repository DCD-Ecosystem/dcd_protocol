#pragma once

#include <fc/exception/exception.hpp>

#include <dcd/vm/backend.hpp>

namespace b1::rodeos {

using wasm_size_t = dcd::vm::wasm_size_t;

template <typename T, std::size_t Align = alignof(T)>
using legacy_ptr = dcd::vm::argument_proxy<T*, Align>;

template <typename T, std::size_t Align = alignof(T)>
using legacy_span = dcd::vm::argument_proxy<dcd::vm::span<T>, Align>;

struct null_terminated_ptr : dcd::vm::span<const char> {
   using base_type = dcd::vm::span<const char>;
   null_terminated_ptr(const char* ptr) : base_type(ptr, strlen(ptr)) {}
};

// wrapper pointer type to keep the validations from being done
template <typename T>
struct unvalidated_ptr {
   static_assert(sizeof(T) == 1); // currently only going to support these for char and const char
      operator T*() { return ptr; }
   T* operator->() { return ptr; }
   T& operator*() { return *ptr; }
   T* ptr;
};

inline size_t legacy_copy_to_wasm(char* dest, size_t dest_size, const char* src, size_t src_size) {
   if (dest_size == 0)
      return src_size;
   auto copy_size = std::min(dest_size, src_size);
   memcpy(dest, src, copy_size);
   return copy_size;
}

template <typename Host, typename Execution_Interface = dcd::vm::execution_interface>
struct type_converter : dcd::vm::type_converter<Host, Execution_Interface> {
   using base_type = dcd::vm::type_converter<Host, Execution_Interface>;
   using base_type::base_type;
   using base_type::from_wasm;

   template <typename T>
   auto from_wasm(const void* ptr) const -> std::enable_if_t<std::is_same_v<T, unvalidated_ptr<const char>>, T> {
      return { static_cast<const char*>(ptr) };
   }

   template <typename T>
   auto from_wasm(void* ptr) const -> std::enable_if_t<std::is_same_v<T, unvalidated_ptr<char>>, T> {
      return { static_cast<char*>(ptr) };
   }

   template <typename T>
   auto from_wasm(void* ptr) const -> std::enable_if_t<std::is_pointer_v<T>, dcd::vm::argument_proxy<T>> {
      this->template validate_pointer<std::remove_pointer_t<T>>(ptr, 1);
      return { ptr };
   }

   EOS_VM_FROM_WASM(null_terminated_ptr, (const void* ptr)) {
      this->validate_null_terminated_pointer(ptr);
      return { static_cast<const char*>(ptr) };
   }
}; // type_converter

template <typename Cls>
using registered_host_functions =
      dcd::vm::registered_host_functions<Cls, dcd::vm::execution_interface,
                                           type_converter<Cls, dcd::vm::execution_interface>>;

} // namespace b1::rodeos
