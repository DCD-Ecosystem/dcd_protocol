#pragma once

#include "for_each_field.hpp"

namespace dcd { namespace operators {

// Defines comparison operators for a reflected struct
#define DCD_COMPARE(...)                                                                                             \
   auto                      dcd_enable_comparison(const __VA_ARGS__&)->bool;                                        \
   using ::dcd::operators::operator==;                                                                               \
   using ::dcd::operators::operator!=;                                                                               \
   using ::dcd::operators::operator<;                                                                                \
   using ::dcd::operators::operator>;                                                                                \
   using ::dcd::operators::operator<=;                                                                               \
   using ::dcd::operators::operator>=;                                                                               \
   using ::dcd::operators::dcd_compare

   template <typename T>
   constexpr auto operator==(const T& lhs, const T& rhs) -> decltype(dcd_enable_comparison(lhs)) {
      bool result = true;
      for_each_field<T>([&](const char*, auto&& member) { result = result && (member(&lhs) == member(&rhs)); });
      return result;
   }
   template <typename T>
   constexpr auto operator!=(const T& lhs, const T& rhs) -> decltype(dcd_enable_comparison(lhs)) {
      return !(lhs == rhs);
   }

   namespace internal_use_do_not_use {
      // This is a worse match than the user-visible overload
      template <typename T, typename U>
      constexpr int dcd_compare(const T& lhs, const U& rhs) {
         if (lhs < rhs)
            return -1;
         else if (rhs < lhs)
            return 1;
         else
            return 0;
      }
   } // namespace internal_use_do_not_use

   template <typename T>
   constexpr auto dcd_compare(const T& lhs, const T& rhs) -> decltype((dcd_enable_comparison(lhs), 0)) {
      int result = 0;
      for_each_field<T>([&](const char*, auto&& member) {
         if (!result) {
            using internal_use_do_not_use::dcd_compare;
            result = dcd_compare(member(&lhs), member(&rhs));
         }
      });
      return result;
   }

   template <typename T>
   constexpr auto operator<(const T& lhs, const T& rhs) -> decltype(dcd_enable_comparison(lhs)) {
      return dcd_compare(lhs, rhs) < 0;
   }
   template <typename T>
   constexpr auto operator>(const T& lhs, const T& rhs) -> decltype(dcd_enable_comparison(lhs)) {
      return dcd_compare(lhs, rhs) > 0;
   }
   template <typename T>
   constexpr auto operator<=(const T& lhs, const T& rhs) -> decltype(dcd_enable_comparison(lhs)) {
      return dcd_compare(lhs, rhs) <= 0;
   }
   template <typename T>
   constexpr auto operator>=(const T& lhs, const T& rhs) -> decltype(dcd_enable_comparison(lhs)) {
      return dcd_compare(lhs, rhs) >= 0;
   }

}} // namespace dcd::operators
