#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] integration_test : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void store( dcd::name from, dcd::name to, uint64_t num );

   struct [[dcd::table("payloads")]] payload {
      uint64_t              key;
      std::vector<uint64_t> data;

      uint64_t primary_key()const { return key; }

      EOSLIB_SERIALIZE( payload, (key)(data) )
   };

   using payloads_table = dcd::multi_index< "payloads"_n,  payload >;

};
