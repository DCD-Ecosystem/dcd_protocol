#pragma once

#include <fc/reflect/variant.hpp>
#include <stdexcept>

namespace dcd { namespace chain {

   enum class backing_store_type {
      CHAINBASE, // A name for regular users. Uses Chainbase.
      ROCKSDB
   };

}} // namespace dcd::chain

namespace fc {
template <>
inline void to_variant(const dcd::chain::backing_store_type& store, fc::variant& v) {
   v = (uint64_t)store;
}
template <>
inline void from_variant(const fc::variant& v, dcd::chain::backing_store_type& store) {
   switch (store = (dcd::chain::backing_store_type)v.as_uint64()) {
      case dcd::chain::backing_store_type::CHAINBASE:
      case dcd::chain::backing_store_type::ROCKSDB:
         return;
   }
   throw std::runtime_error("Invalid backing store name: " + v.as_string());
}
} // namespace fc
