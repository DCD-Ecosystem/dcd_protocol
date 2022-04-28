#pragma once

#include <dcd/dcd.hpp>
#include <vector>

class [[dcd::contract]] deferred_test : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void defercall( dcd::name payer, uint64_t sender_id, dcd::name contract, uint64_t payload );

   [[dcd::action]]
   void delayedcall( dcd::name payer, uint64_t sender_id, dcd::name contract,
                     uint64_t payload, uint32_t delay_sec, bool replace_existing );

   [[dcd::action]]
   void deferfunc( uint64_t payload );
   using deferfunc_action = dcd::action_wrapper<"deferfunc"_n, &deferred_test::deferfunc>;

   [[dcd::action]]
   void inlinecall( dcd::name contract, dcd::name authorizer, uint64_t payload );

   [[dcd::action]]
   void fail();

   [[dcd::on_notify("dcd::onerror")]]
   void on_error( uint128_t sender_id, dcd::ignore<std::vector<char>> sent_trx );
};
