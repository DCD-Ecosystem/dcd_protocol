#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] ram_restrictions_test : public dcd::contract {
public:
   struct [[dcd::table]] data {
      uint64_t           key;
      std::vector<char>  value;

      uint64_t primary_key() const { return key; }
   };

   typedef dcd::multi_index<"tablea"_n, data> tablea;
   typedef dcd::multi_index<"tableb"_n, data> tableb;

public:
   using dcd::contract::contract;

   [[dcd::action]]
   void noop();

   [[dcd::action]]
   void setdata( uint32_t len1, uint32_t len2, dcd::name payer );

   [[dcd::action]]
   void notifysetdat( dcd::name acctonotify, uint32_t len1, uint32_t len2, dcd::name payer );

   [[dcd::on_notify("tester2::notifysetdat")]]
   void on_notify_setdata( dcd::name acctonotify, uint32_t len1, uint32_t len2, dcd::name payer );

   [[dcd::action]]
   void senddefer( uint64_t senderid, dcd::name payer );

   [[dcd::action]]
   void notifydefer( dcd::name acctonotify, uint64_t senderid, dcd::name payer );

   [[dcd::on_notify("tester2::notifydefer")]]
   void on_notifydefer( dcd::name acctonotify, uint64_t senderid, dcd::name payer );

};
