#pragma once
#include <dcd/testing/tester.hpp>

struct dummy_action {
   static dcd::chain::name get_name() {
      using namespace dcd::chain::literals;
      return "dummyaction"_n;
   }
   static dcd::chain::name get_account() {
      using namespace dcd::chain::literals;
      return "testapi"_n;
   }

   char     a; // 1
   uint64_t b; // 8
   int32_t  c; // 4
};

struct cf_action {
   static dcd::chain::name get_name() {
      using namespace dcd::chain::literals;
      return "cfaction"_n;
   }
   static dcd::chain::name get_account() {
      using namespace dcd::chain::literals;
      return "testapi"_n;
   }

   uint32_t payload = 100;
   uint32_t cfd_idx = 0; // context free data index
};

FC_REFLECT(dummy_action, (a)(b)(c))
FC_REFLECT(cf_action, (payload)(cfd_idx))

#define DUMMY_ACTION_DEFAULT_A 0x45
#define DUMMY_ACTION_DEFAULT_B 0xab11cd1244556677
#define DUMMY_ACTION_DEFAULT_C 0x7451ae12

std::vector<dcd::chain::signed_block_ptr> deploy_test_api(dcd::testing::tester& chain);
dcd::chain::transaction_trace_ptr push_test_cfd_transaction(dcd::testing::tester& chain);

struct scoped_temp_path {
   boost::filesystem::path path;
   scoped_temp_path() {
      path = boost::filesystem::unique_path();
      if (boost::unit_test::framework::master_test_suite().argc >= 2) {
         path += boost::unit_test::framework::master_test_suite().argv[1];
      }
   }
   ~scoped_temp_path() {
      boost::filesystem::remove_all(path);
   }
};
