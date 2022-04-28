#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <dcd/testing/tester.hpp>
#include <dcd/chain/abi_serializer.hpp>
#include <dcd/chain/wasm_dcd_constraints.hpp>
//#include <dcd/chain/resource_limits.hpp>
#include <dcd/chain/exceptions.hpp>
#include <dcd/chain/wast_to_wasm.hpp>
#include <dcd/chain_plugin/chain_plugin.hpp>

#include <contracts.hpp>

#include <fc/io/fstream.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>
#include <fc/io/json.hpp>

#include <array>
#include <utility>

#include <dcd/testing/backing_store_tester_macros.hpp>

using namespace dcd;
using namespace dcd::chain;
using namespace dcd::testing;
using namespace fc;

BOOST_AUTO_TEST_SUITE(get_table_seckey_tests)

using backing_store_ts = boost::mpl::list<TESTER, ROCKSDB_TESTER>;

transaction_trace_ptr
issue_tokens( TESTER& t, account_name issuer, account_name to, const asset& amount,
              std::string memo = "", account_name token_contract = "dcd"_n )
{
   signed_transaction trx;

   trx.actions.emplace_back( t.get_action( token_contract, "issue"_n,
                                           vector<permission_level>{{issuer, config::active_name}},
                                           mutable_variant_object()
                                             ("to", issuer.to_string())
                                             ("quantity", amount)
                                             ("memo", memo)
   ) );

   trx.actions.emplace_back( t.get_action( token_contract, "transfer"_n,
                                           vector<permission_level>{{issuer, config::active_name}},
                                           mutable_variant_object()
                                             ("from", issuer.to_string())
                                             ("to", to.to_string())
                                             ("quantity", amount)
                                             ("memo", memo)
   ) );

   t.set_transaction_headers(trx);
   trx.sign( t.get_private_key( issuer, "active" ), t.control->get_chain_id()  );
   return t.push_transaction( trx );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( get_table_next_key_test, TESTER_T, backing_store_ts) { try {
   TESTER_T t;
   t.create_account("test"_n);

   // setup contract and abi
   t.set_code( "test"_n, contracts::get_table_seckey_test_wasm() );
   t.set_abi( "test"_n, contracts::get_table_seckey_test_abi().data() );
   t.produce_block();

   chain_apis::read_only plugin(*(t.control), {}, fc::microseconds::maximum());
   chain_apis::read_only::get_table_rows_params params = []{
      chain_apis::read_only::get_table_rows_params params{};
      params.json=true;
      params.code="test"_n;
      params.scope="test";
      params.limit=1;
      return params;
   }();

   params.table = "numobjs"_n;


   // name secondary key type
   t.push_action("test"_n, "addnumobj"_n, "test"_n, mutable_variant_object()("input", 2)("nm", "a"));
   t.push_action("test"_n, "addnumobj"_n, "test"_n, mutable_variant_object()("input", 5)("nm", "b"));
   t.push_action("test"_n, "addnumobj"_n, "test"_n, mutable_variant_object()("input", 7)("nm", "c"));

   params.table = "numobjs"_n;
   params.key_type = "name";
   params.limit = 10;
   params.index_position = "6";
   params.lower_bound = "a";
   params.upper_bound = "a";
   auto res_nm = plugin.get_table_rows(params);
   BOOST_REQUIRE(res_nm.rows.size() == 1);

   params.lower_bound = "a";
   params.upper_bound = "b";
   res_nm = plugin.get_table_rows(params);
   BOOST_REQUIRE(res_nm.rows.size() == 2);

   params.lower_bound = "a";
   params.upper_bound = "c";
   res_nm = plugin.get_table_rows(params);
   BOOST_REQUIRE(res_nm.rows.size() == 3);

   t.push_action("test"_n, "addnumobj"_n, "test"_n, mutable_variant_object()("input", 8)("nm", "1111"));
   t.push_action("test"_n, "addnumobj"_n, "test"_n, mutable_variant_object()("input", 9)("nm", "2222"));
   t.push_action("test"_n, "addnumobj"_n, "test"_n, mutable_variant_object()("input", 10)("nm", "3333"));

   params.lower_bound = "1111";
   params.upper_bound = "3333";
   res_nm = plugin.get_table_rows(params);
   BOOST_REQUIRE(res_nm.rows.size() == 3);

   params.lower_bound = "2222";
   params.upper_bound = "3333";
   res_nm = plugin.get_table_rows(params);
   BOOST_REQUIRE(res_nm.rows.size() == 2);

} FC_LOG_AND_RETHROW() }/// get_table_next_key_test

BOOST_AUTO_TEST_CASE_TEMPLATE( get_table_next_key_reverse_test, TESTER_T, backing_store_ts) { try {
   TESTER_T t;
   t.produce_blocks(2);

   t.create_accounts({ "dcd.token"_n, "dcd.ram"_n, "dcd.ramfee"_n, "dcd.stake"_n,
      "dcd.bpay"_n, "dcd.vpay"_n, "dcd.saving"_n, "dcd.names"_n });

   std::vector<account_name> accs{"inita"_n, "initb"_n, "initc"_n, "initd"_n};
   t.create_accounts(accs);
   t.produce_block();

   t.set_code( "dcd"_n, contracts::dcd_token_wasm() );
   t.set_abi( "dcd"_n, contracts::dcd_token_abi().data() );
   t.produce_blocks(1);

   // create currency
   auto act = mutable_variant_object()
         ("issuer",       "dcd")
         ("maximum_supply", dcd::chain::asset::from_string("1000000000.0000 DCD"));
   t.push_action("dcd"_n, "create"_n, "dcd"_n, act );

   // issue
   for (account_name a: accs) {
      issue_tokens( t, config::system_account_name, a, dcd::chain::asset::from_string("999.0000 DCD") );
   }
   t.produce_blocks(1);

   // iterate over scope
   dcd::chain_apis::read_only plugin(*(t.control), {}, fc::microseconds::maximum());
   dcd::chain_apis::read_only::get_table_by_scope_params param{"dcd"_n, "accounts"_n, "inita", "", 10};

   param.lower_bound = "a";
   param.upper_bound = "z";
   param.reverse = true;
   dcd::chain_apis::read_only::get_table_by_scope_result result = plugin.read_only::get_table_by_scope(param);
   BOOST_REQUIRE_EQUAL(5u, result.rows.size());
   BOOST_REQUIRE_EQUAL("", result.more);

   BOOST_REQUIRE_EQUAL(name("initd"_n), result.rows[0].scope);
   BOOST_REQUIRE_EQUAL(name("initc"_n), result.rows[1].scope);
   BOOST_REQUIRE_EQUAL(name("initb"_n), result.rows[2].scope);
   BOOST_REQUIRE_EQUAL(name("inita"_n), result.rows[3].scope);
   BOOST_REQUIRE_EQUAL(name("dcd"_n), result.rows[4].scope);

} FC_LOG_AND_RETHROW() } /// get_scope_test

BOOST_AUTO_TEST_SUITE_END()
