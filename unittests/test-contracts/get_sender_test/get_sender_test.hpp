#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] get_sender_test : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void assertsender( dcd::name expected_sender );
   using assertsender_action = dcd::action_wrapper<"assertsender"_n, &get_sender_test::assertsender>;

   [[dcd::action]]
   void sendinline( dcd::name to, dcd::name expected_sender );

   [[dcd::action]]
   void notify( dcd::name to, dcd::name expected_sender, bool send_inline );

   [[dcd::on_notify("*::notify")]]
   void on_notify( dcd::name to, dcd::name expected_sender, bool send_inline );

};
