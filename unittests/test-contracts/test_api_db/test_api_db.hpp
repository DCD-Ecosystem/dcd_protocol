#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] test_api_db : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action("pg")]]
   void primary_i64_general();

   [[dcd::action("pl")]]
   void primary_i64_lowerbound();

   [[dcd::action("pu")]]
   void primary_i64_upperbound();

   [[dcd::action("s1g")]]
   void idx64_general();

   [[dcd::action("s1l")]]
   void idx64_lowerbound();

   [[dcd::action("s1u")]]
   void idx64_upperbound();

   [[dcd::action("tia")]]
   void test_invalid_access( dcd::name code, uint64_t val, uint32_t index, bool store );

   [[dcd::action("sdnancreate")]]
   void idx_double_nan_create_fail();

   [[dcd::action("sdnanmodify")]]
   void idx_double_nan_modify_fail();

   [[dcd::action("sdnanlookup")]]
   void idx_double_nan_lookup_fail( uint32_t lookup_type );

   [[dcd::action("sk32align")]]
   void misaligned_secondary_key256_tests();

};
