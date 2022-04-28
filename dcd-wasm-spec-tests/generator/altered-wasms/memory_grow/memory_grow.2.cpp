#include <dcd/dcd.hpp>

extern "C" {
   int32_t _grow(int32_t) {
      return 0;
   }

   void sub_apply_0() {
      int32_t x1 = _grow((int32_t)0);
      dcd::check(x1 == (int32_t)1, "_grow fail 1");

      int32_t x2 = _grow((int32_t)1);
      dcd::check(x2 == (int32_t)1, "_grow fail 2");

      int32_t x3 = _grow((int32_t)1);
      dcd::check(x3 == (int32_t)2, "_grow fail 3");

      int32_t x4 = _grow((int32_t)2);
      dcd::check(x4 == (int32_t)3, "_grow fail 4");

      int32_t x5 = _grow((int32_t)6);
      dcd::check(x5 == (int32_t)5, "_grow fail 5");

      int32_t x6 = _grow((int32_t)0);
      dcd::check(x6 == (int32_t)11, "_grow fail 6");

      int32_t x7 = _grow((int32_t)1);
      dcd::check(x7 == (int32_t)4294967295, "_grow fail 7");

      int32_t x8 = _grow((int32_t)65536);
      dcd::check(x8 == (int32_t)4294967295, "_grow fail 8");

   }
   void apply(uint64_t, uint64_t, uint64_t test_to_run) {
      volatile uint64_t* r = (uint64_t*)0;
      *r = 0;
      switch(test_to_run) {
         case 0:
            sub_apply_0();
            break;
      }
   }
}
