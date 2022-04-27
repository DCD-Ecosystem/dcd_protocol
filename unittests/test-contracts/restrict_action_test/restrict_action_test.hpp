#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] restrict_action_test : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void noop( );

   [[dcd::action]]
   void sendinline( dcd::name authorizer );

   [[dcd::action]]
   void senddefer( dcd::name authorizer, uint32_t senderid );


   [[dcd::action]]
   void notifyinline( dcd::name acctonotify, dcd::name authorizer );

   [[dcd::action]]
   void notifydefer( dcd::name acctonotify, dcd::name authorizer, uint32_t senderid );

   [[dcd::on_notify("testacc::notifyinline")]]
   void on_notify_inline( dcd::name acctonotify, dcd::name authorizer );

   [[dcd::on_notify("testacc::notifydefer")]]
   void on_notify_defer( dcd::name acctonotify, dcd::name authorizer, uint32_t senderid );
};
