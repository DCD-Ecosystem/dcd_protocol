#pragma once

#include <dcd/dcd.hpp>
#include <dcd/singleton.hpp>
#include <dcd/asset.hpp>

// Extacted from dcd.token contract:
namespace dcd {
   class [[dcd::contract("dcd.token")]] token : public dcd::contract {
   public:
      using dcd::contract::contract;

      [[dcd::action]]
      void transfer( dcd::name        from,
                     dcd::name        to,
                     dcd::asset       quantity,
                     const std::string& memo );
      using transfer_action = dcd::action_wrapper<"transfer"_n, &token::transfer>;
   };
}

// This contract:
class [[dcd::contract]] proxy : public dcd::contract {
public:
   proxy( dcd::name self, dcd::name first_receiver, dcd::datastream<const char*> ds );

   [[dcd::action]]
   void setowner( dcd::name owner, uint32_t delay );

   [[dcd::on_notify("dcd.token::transfer")]]
   void on_transfer( dcd::name        from,
                     dcd::name        to,
                     dcd::asset       quantity,
                     const std::string& memo );

   [[dcd::on_notify("dcd::onerror")]]
   void on_error( uint128_t sender_id, dcd::ignore<std::vector<char>> sent_trx );

   struct [[dcd::table]] config {
      dcd::name owner;
      uint32_t    delay   = 0;
      uint32_t    next_id = 0;

      DCDLIB_SERIALIZE( config, (owner)(delay)(next_id) )
   };

   using config_singleton = dcd::singleton< "config"_n,  config >;

protected:
   config_singleton _config;
};
