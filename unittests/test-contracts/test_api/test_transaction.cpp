#include <dcd/action.hpp>
#include <dcd/crypto.hpp>
#include <dcd/transaction.hpp>

#include "test_api.hpp"

#pragma pack(push, 1)
template <uint64_t ACCOUNT, uint64_t NAME>
struct test_action_action {
   static dcd::name get_account() {
      return dcd::name{ACCOUNT};
   }

   static dcd::name get_name() {
      return dcd::name{NAME};
   }

   std::vector<char> data;

   template <typename DataStream>
   friend DataStream& operator<< ( DataStream& ds, const test_action_action& a ) {
      for ( auto c : a.data )
         ds << c;
      return ds;
   }
};


template <uint64_t ACCOUNT, uint64_t NAME>
struct test_dummy_action {
   static dcd::name get_account() {
      return dcd::name{ACCOUNT};
   }

   static dcd::name get_name() {
      return dcd::name{NAME};
   }
   char a;
   unsigned long long b;
   int32_t c;

   template <typename DataStream>
   friend DataStream& operator<< ( DataStream& ds, const test_dummy_action& da ) {
      ds << da.a;
      ds << da.b;
      ds << da.c;
      return ds;
   }

   template <typename DataStream>
   friend DataStream& operator>> ( DataStream& ds, test_dummy_action& da ) {
      ds >> da.a;
      ds >> da.b;
      ds >> da.c;
      return ds;
   }
};
#pragma pack(pop)

void copy_data( char* data, size_t data_len, std::vector<char>& data_out ) {
   for (unsigned int i=0; i < data_len; i++)
      data_out.push_back(data[i]);
}

void test_transaction::send_action() {
   using namespace dcd;
   test_dummy_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "read_action_normal" )> test_action =
      { DUMMY_ACTION_DEFAULT_A, DUMMY_ACTION_DEFAULT_B, DUMMY_ACTION_DEFAULT_C };

   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };
   action act( permissions, name{"testapi"}, name{WASM_TEST_ACTION( "test_action", "read_action_normal" )}, test_action );

   act.send();
}

void test_transaction::send_action_empty() {
   using namespace dcd;
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "assert_true" )> test_action;

   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };
   action act( permissions, name{"testapi"}, name{WASM_TEST_ACTION( "test_action", "assert_true" )}, test_action );

   act.send();
}

/**
 * cause failure due to a large action payload
 */
void test_transaction::send_action_large() {
   using namespace dcd;
   static char large_message[8 * 1024];
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "read_action_normal" )> test_action;
   copy_data( large_message, 8*1024, test_action.data );

   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };
   action act( permissions, name{"testapi"}, name{WASM_TEST_ACTION("test_action", "read_action_normal")}, test_action );

   act.send();
   dcd_assert( false, "send_message_large() should've thrown an error" );
}

/**
 * send an inline action that is 4K
 */
void test_transaction::send_action_4k() {
   using namespace dcd;
   static char large_message[4 * 1024];
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "test_action_ordinal4" )> test_action;
   copy_data( large_message, 4*1024, test_action.data );

   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };
   action act( permissions, name{"testapi"}, name{WASM_TEST_ACTION("test_action", "test_action_ordinal4")}, test_action );

   act.send();
}

/**
 * cause failure due recursive loop
 */
void test_transaction::send_action_recurse() {
   using namespace dcd;
   char buffer[1024];
   dcd::read_action_data( buffer, 1024 );

   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_transaction", "send_action_recurse" )> test_action;
   copy_data( buffer, 1024, test_action.data );

   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };
   action act( permissions, name{"testapi"}, name{WASM_TEST_ACTION( "test_transaction", "send_action_recurse" )}, test_action );

   act.send();
}

/**
 * cause failure due to inline TX failure
 */
void test_transaction::send_action_inline_fail() {
   using namespace dcd;
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "assert_false" )> test_action;

   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };
   action act( permissions, name{"testapi"}, name{WASM_TEST_ACTION( "test_action", "assert_false" )}, test_action );

   act.send();
}

void test_transaction::test_tapos_block_prefix() {
   using namespace dcd;
   int tbp;
   dcd::read_action_data( (char*)&tbp, sizeof(int) );
   dcd_assert( tbp == dcd::tapos_block_prefix(), "tapos_block_prefix does not match" );
}

void test_transaction::test_tapos_block_num() {
   using namespace dcd;
   int tbn;
   dcd::read_action_data( (char*)&tbn, sizeof(int) );
   dcd_assert( tbn == dcd::tapos_block_num(), "tapos_block_num does not match" );
}

void test_transaction::test_read_transaction() {
   using namespace dcd;
   checksum256 h;
   auto size = dcd::transaction_size();
   char buf[size];
   uint32_t read = dcd::read_transaction( buf, size );
   dcd_assert( size == read, "read_transaction failed");
   h = dcd::sha256(buf, read);
   print(h);
}

void test_transaction::test_transaction_size() {
   using namespace dcd;
   uint32_t trans_size = 0;
   dcd::read_action_data( (char*)&trans_size, sizeof(uint32_t) );
   print( "size: ", dcd::transaction_size() );
   dcd_assert( trans_size == dcd::transaction_size(), "transaction size does not match" );
}

void test_transaction::send_transaction(uint64_t receiver, uint64_t, uint64_t) {
   using namespace dcd;
   dummy_action payload = { DUMMY_ACTION_DEFAULT_A, DUMMY_ACTION_DEFAULT_B, DUMMY_ACTION_DEFAULT_C };

   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "read_action_normal" )> test_action;
   copy_data( (char*)&payload, sizeof(dummy_action), test_action.data );

   auto trx = transaction();
   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };

   trx.actions.emplace_back(permissions, name{"testapi"}, name{WASM_TEST_ACTION( "test_action", "read_action_normal" )}, test_action);
   trx.send( 0, name{receiver} );
}

void test_transaction::send_action_sender( uint64_t receiver, uint64_t, uint64_t ) {
   using namespace dcd;
   uint64_t cur_send;
   dcd::read_action_data( &cur_send, sizeof(name) );

   auto trx = transaction();
   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };

   trx.actions.emplace_back(permissions, name{"testapi"}, name{WASM_TEST_ACTION( "test_action", "test_current_sender" )}, &cur_send);
   trx.send( 0, name{receiver} );
}

void test_transaction::send_transaction_empty( uint64_t receiver, uint64_t, uint64_t ) {
   using namespace dcd;
   auto trx = transaction();
   trx.send( 0, name{receiver} );

   dcd_assert( false, "send_transaction_empty() should've thrown an error" );
}

void test_transaction::send_transaction_trigger_error_handler( uint64_t receiver, uint64_t, uint64_t ) {
   using namespace dcd;
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "assert_false" )> test_action;

   auto trx = transaction();
   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };

   trx.actions.emplace_back( permissions, name{"testapi"}, name{WASM_TEST_ACTION("test_action", "assert_false")}, test_action );
   trx.send(0, name{receiver});
}

void test_transaction::assert_false_error_handler( const dcd::transaction& dtrx ) {
   dcd_assert( dtrx.actions.size() == 1, "transaction should only have one action" );
   dcd_assert( dtrx.actions[0].account == "testapi"_n, "transaction has wrong code" );
   dcd_assert( dtrx.actions[0].name.value == WASM_TEST_ACTION("test_action", "assert_false"), "transaction has wrong name" );
   dcd_assert( dtrx.actions[0].authorization.size() == 1, "action should only have one authorization" );
   dcd_assert( dtrx.actions[0].authorization[0].actor == "testapi"_n, "action's authorization has wrong actor" );
   dcd_assert( dtrx.actions[0].authorization[0].permission == "active"_n, "action's authorization has wrong permission" );
}

/**
 * cause failure due to a large transaction size
 */
void test_transaction::send_transaction_large( uint64_t receiver, uint64_t, uint64_t ) {
   using namespace dcd;
   auto trx = transaction();
   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };
   for (int i = 0; i < 32; i ++) {
      char large_message[1024];
      test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_action", "read_action_normal" )> test_action;
      copy_data( large_message, 1024, test_action.data );
      trx.actions.emplace_back( permissions, name{"testapi"}, name{WASM_TEST_ACTION("test_action", "read_action_normal")}, test_action );
   }

   trx.send( 0, name{receiver} );

   dcd_assert( false, "send_transaction_large() should've thrown an error" );
}

/**
 * deferred transaction
 */
void test_transaction::deferred_print() {
   dcd::print("deferred executed\n");
}

void test_transaction::send_deferred_transaction( uint64_t receiver, uint64_t, uint64_t ) {
   using namespace dcd;
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_transaction", "deferred_print" )> test_action;

   auto trx = transaction();
   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };

   trx.actions.emplace_back( permissions, name{"testapi"}, name{ WASM_TEST_ACTION("test_transaction", "deferred_print" )}, test_action );
   trx.delay_sec = 2;
   trx.send( 0xffffffffffffffff, name{receiver} );
}

void test_transaction::send_deferred_transaction_4k_action( uint64_t receiver, uint64_t, uint64_t ) {
   using namespace dcd;
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_transaction", "send_action_4k" )> test_action;

   auto trx = transaction();
   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };

   trx.actions.emplace_back( permissions, name{"testapi"}, name{ WASM_TEST_ACTION("test_transaction", "send_action_4k" )}, test_action );
   trx.delay_sec = 2;
   trx.send( 0xffffffffffffffff, name{receiver} );
}

void test_transaction::send_deferred_transaction_replace( uint64_t receiver, uint64_t, uint64_t ) {
   using namespace dcd;
   test_action_action<"testapi"_n.value, WASM_TEST_ACTION( "test_transaction", "deferred_print" )> test_action;

   auto trx = transaction();
   std::vector<permission_level> permissions = { {"testapi"_n, "active"_n} };

   trx.actions.emplace_back( permissions, name{"testapi"}, name{WASM_TEST_ACTION( "test_transaction", "deferred_print" )}, test_action );
   trx.delay_sec = 2;
   trx.send( 0xffffffffffffffff, name{receiver}, true );
}

void test_transaction::send_deferred_tx_with_dtt_action() {
   using namespace dcd;
   dtt_action dtt_act;
   dcd::read_action_data( &dtt_act, dcd::action_data_size() );

   action deferred_act;
   deferred_act.account = name{dtt_act.deferred_account};
   deferred_act.name = name{dtt_act.deferred_action};
   deferred_act.authorization = std::vector<permission_level>{ {"testapi"_n, name{dtt_act.permission_name}} };

   auto trx = transaction();
   trx.actions.emplace_back(deferred_act);
   trx.delay_sec = dtt_act.delay_sec;
   trx.send( 0xffffffffffffffff, name{dtt_act.payer}, true );
}


void test_transaction::cancel_deferred_transaction_success() {
   using namespace dcd;
   auto r = dcd::cancel_deferred( 0xffffffffffffffff ); //use the same id (0) as in send_deferred_transaction
   dcd_assert( (bool)r, "transaction was not found" );
}

void test_transaction::cancel_deferred_transaction_not_found() {
   using namespace dcd;
   auto r = dcd::cancel_deferred( 0xffffffffffffffff ); //use the same id (0) as in send_deferred_transaction
   dcd_assert( !r, "transaction was canceled, whild should not be found" );
}

void test_transaction::send_cf_action() {
   using namespace dcd;
   action act( std::vector<permission_level>{}, "dummy"_n, "event1"_n, std::vector<char>{} );
   act.send_context_free();
}

void test_transaction::send_cf_action_fail() {
   using namespace dcd;
   action act( std::vector<permission_level>{{"dummy"_n, "active"_n}}, "dummy"_n, "event1"_n, std::vector<char>{} );
   act.send_context_free();
   dcd_assert( false, "send_cfa_action_fail() should've thrown an error" );
}

void test_transaction::stateful_api() {
   char buf[4] = {1};
   db_store_i64( dcd::name{"testtrans"}.value, dcd::name{"table"}.value, dcd::name{"testtrans"}.value, 0, buf, 4 );
}

void test_transaction::context_free_api() {
   char buf[128] = {0};
   dcd::get_context_free_data( 0, buf, sizeof(buf) );
}

void test_transaction::repeat_deferred_transaction( uint64_t receiver, uint64_t code, uint64_t action ) {
   using namespace dcd;

   uint128_t sender_id = 0;

   uint32_t payload = unpack_action_data<uint32_t>();
   print("repeat_deferred_transaction called: payload = ", payload);

   bool res = dcd::cancel_deferred( sender_id );

   print("\nrepeat_deferred_transaction cancelled trx with sender_id = ", sender_id, ", result is ", res);

   if( payload == 0 ) return;

   --payload;
   transaction trx;
   std::vector<permission_level> permissions = { {name{receiver}, "active"_n} };

   trx.actions.emplace_back( permissions, name{code}, name{action}, payload );
   trx.send( sender_id, dcd::name{receiver} );
}
