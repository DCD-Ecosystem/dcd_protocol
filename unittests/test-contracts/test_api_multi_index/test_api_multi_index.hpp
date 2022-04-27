#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] test_api_multi_index : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action("s1g")]]
   void idx64_general();

   [[dcd::action("s1store")]]
   void idx64_store_only();

   [[dcd::action("s1check")]]
   void idx64_check_without_storing();

   [[dcd::action("s1findfail1")]]
   void idx64_require_find_fail();

   [[dcd::action("s1findfail2")]]
   void idx64_require_find_fail_with_msg();

   [[dcd::action("s1findfail3")]]
   void idx64_require_find_sk_fail();

   [[dcd::action("s1findfail4")]]
   void idx64_require_find_sk_fail_with_msg();

   [[dcd::action("s1pkend")]]
   void idx64_pk_iterator_exceed_end();

   [[dcd::action("s1skend")]]
   void idx64_sk_iterator_exceed_end();

   [[dcd::action("s1pkbegin")]]
   void idx64_pk_iterator_exceed_begin();

   [[dcd::action("s1skbegin")]]
   void idx64_sk_iterator_exceed_begin();

   [[dcd::action("s1pkref")]]
   void idx64_pass_pk_ref_to_other_table();

   [[dcd::action("s1skref")]]
   void idx64_pass_sk_ref_to_other_table();

   [[dcd::action("s1pkitrto")]]
   void idx64_pass_pk_end_itr_to_iterator_to();

   [[dcd::action("s1pkmodify")]]
   void idx64_pass_pk_end_itr_to_modify();

   [[dcd::action("s1pkerase")]]
   void idx64_pass_pk_end_itr_to_erase();

   [[dcd::action("s1skitrto")]]
   void idx64_pass_sk_end_itr_to_iterator_to();

   [[dcd::action("s1skmodify")]]
   void idx64_pass_sk_end_itr_to_modify();

   [[dcd::action("s1skerase")]]
   void idx64_pass_sk_end_itr_to_erase();

   [[dcd::action("s1modpk")]]
   void idx64_modify_primary_key();

   [[dcd::action("s1exhaustpk")]]
   void idx64_run_out_of_avl_pk();

   [[dcd::action("s1skcache")]]
   void idx64_sk_cache_pk_lookup();

   [[dcd::action("s1pkcache")]]
   void idx64_pk_cache_sk_lookup();

   [[dcd::action("s2g")]]
   void idx128_general();

   [[dcd::action("s2store")]]
   void idx128_store_only();

   [[dcd::action("s2check")]]
   void idx128_check_without_storing();

   [[dcd::action("s2autoinc")]]
   void idx128_autoincrement_test();

   [[dcd::action("s2autoinc1")]]
   void idx128_autoincrement_test_part1();

   [[dcd::action("s2autoinc2")]]
   void idx128_autoincrement_test_part2();

   [[dcd::action("s3g")]]
   void idx256_general();

   [[dcd::action("sdg")]]
   void idx_double_general();

   [[dcd::action("sldg")]]
   void idx_long_double_general();

};
