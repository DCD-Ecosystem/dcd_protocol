#include <dcd/dcd.hpp>

#include "test_api.hpp"

void test_types::types_size() {

   dcd_assert( sizeof(int64_t)   ==  8, "int64_t size != 8"   );
   dcd_assert( sizeof(uint64_t)  ==  8, "uint64_t size != 8"  );
   dcd_assert( sizeof(uint32_t)  ==  4, "uint32_t size != 4"  );
   dcd_assert( sizeof(int32_t)   ==  4, "int32_t size != 4"   );
   dcd_assert( sizeof(uint128_t) == 16, "uint128_t size != 16");
   dcd_assert( sizeof(int128_t)  == 16, "int128_t size != 16" );
   dcd_assert( sizeof(uint8_t)   ==  1, "uint8_t size != 1"   );

   dcd_assert( sizeof(dcd::name) ==  8, "name size !=  8");
}

void test_types::char_to_symbol() {

   dcd_assert( dcd::name::char_to_value('1') ==  1, "dcd::char_to_symbol('1') !=  1" );
   dcd_assert( dcd::name::char_to_value('2') ==  2, "dcd::char_to_symbol('2') !=  2" );
   dcd_assert( dcd::name::char_to_value('3') ==  3, "dcd::char_to_symbol('3') !=  3" );
   dcd_assert( dcd::name::char_to_value('4') ==  4, "dcd::char_to_symbol('4') !=  4" );
   dcd_assert( dcd::name::char_to_value('5') ==  5, "dcd::char_to_symbol('5') !=  5" );
   dcd_assert( dcd::name::char_to_value('a') ==  6, "dcd::char_to_symbol('a') !=  6" );
   dcd_assert( dcd::name::char_to_value('b') ==  7, "dcd::char_to_symbol('b') !=  7" );
   dcd_assert( dcd::name::char_to_value('c') ==  8, "dcd::char_to_symbol('c') !=  8" );
   dcd_assert( dcd::name::char_to_value('d') ==  9, "dcd::char_to_symbol('d') !=  9" );
   dcd_assert( dcd::name::char_to_value('e') == 10, "dcd::char_to_symbol('e') != 10" );
   dcd_assert( dcd::name::char_to_value('f') == 11, "dcd::char_to_symbol('f') != 11" );
   dcd_assert( dcd::name::char_to_value('g') == 12, "dcd::char_to_symbol('g') != 12" );
   dcd_assert( dcd::name::char_to_value('h') == 13, "dcd::char_to_symbol('h') != 13" );
   dcd_assert( dcd::name::char_to_value('i') == 14, "dcd::char_to_symbol('i') != 14" );
   dcd_assert( dcd::name::char_to_value('j') == 15, "dcd::char_to_symbol('j') != 15" );
   dcd_assert( dcd::name::char_to_value('k') == 16, "dcd::char_to_symbol('k') != 16" );
   dcd_assert( dcd::name::char_to_value('l') == 17, "dcd::char_to_symbol('l') != 17" );
   dcd_assert( dcd::name::char_to_value('m') == 18, "dcd::char_to_symbol('m') != 18" );
   dcd_assert( dcd::name::char_to_value('n') == 19, "dcd::char_to_symbol('n') != 19" );
   dcd_assert( dcd::name::char_to_value('o') == 20, "dcd::char_to_symbol('o') != 20" );
   dcd_assert( dcd::name::char_to_value('p') == 21, "dcd::char_to_symbol('p') != 21" );
   dcd_assert( dcd::name::char_to_value('q') == 22, "dcd::char_to_symbol('q') != 22" );
   dcd_assert( dcd::name::char_to_value('r') == 23, "dcd::char_to_symbol('r') != 23" );
   dcd_assert( dcd::name::char_to_value('s') == 24, "dcd::char_to_symbol('s') != 24" );
   dcd_assert( dcd::name::char_to_value('t') == 25, "dcd::char_to_symbol('t') != 25" );
   dcd_assert( dcd::name::char_to_value('u') == 26, "dcd::char_to_symbol('u') != 26" );
   dcd_assert( dcd::name::char_to_value('v') == 27, "dcd::char_to_symbol('v') != 27" );
   dcd_assert( dcd::name::char_to_value('w') == 28, "dcd::char_to_symbol('w') != 28" );
   dcd_assert( dcd::name::char_to_value('x') == 29, "dcd::char_to_symbol('x') != 29" );
   dcd_assert( dcd::name::char_to_value('y') == 30, "dcd::char_to_symbol('y') != 30" );
   dcd_assert( dcd::name::char_to_value('z') == 31, "dcd::char_to_symbol('z') != 31" );

   for(unsigned char i = 0; i<255; i++) {
      if( (i >= 'a' && i <= 'z') || (i >= '1' || i <= '5') ) continue;
      dcd_assert( dcd::name::char_to_value((char)i) == 0, "dcd::char_to_symbol() != 0" );
   }
}

void test_types::string_to_name() {
   return;
   dcd_assert( dcd::name("a") == "a"_n, "dcd::string_to_name(a)" );
   dcd_assert( dcd::name("ba") == "ba"_n, "dcd::string_to_name(ba)" );
   dcd_assert( dcd::name("cba") == "cba"_n, "dcd::string_to_name(cba)" );
   dcd_assert( dcd::name("dcba") == "dcba"_n, "dcd::string_to_name(dcba)" );
   dcd_assert( dcd::name("edcba") == "edcba"_n, "dcd::string_to_name(edcba)" );
   dcd_assert( dcd::name("fedcba") == "fedcba"_n, "dcd::string_to_name(fedcba)" );
   dcd_assert( dcd::name("gfedcba") == "gfedcba"_n, "dcd::string_to_name(gfedcba)" );
   dcd_assert( dcd::name("hgfedcba") == "hgfedcba"_n, "dcd::string_to_name(hgfedcba)" );
   dcd_assert( dcd::name("ihgfedcba") == "ihgfedcba"_n, "dcd::string_to_name(ihgfedcba)" );
   dcd_assert( dcd::name("jihgfedcba") == "jihgfedcba"_n, "dcd::string_to_name(jihgfedcba)" );
   dcd_assert( dcd::name("kjihgfedcba") == "kjihgfedcba"_n, "dcd::string_to_name(kjihgfedcba)" );
   dcd_assert( dcd::name("lkjihgfedcba") == "lkjihgfedcba"_n, "dcd::string_to_name(lkjihgfedcba)" );
   dcd_assert( dcd::name("mlkjihgfedcba") == "mlkjihgfedcba"_n, "dcd::string_to_name(mlkjihgfedcba)" );
}
