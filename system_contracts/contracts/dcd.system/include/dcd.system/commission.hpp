#pragma once

#include <dcd/asset.hpp>
#include <dcd/binary_extension.hpp>
#include <dcd/privileged.hpp>
#include <dcd/producer_schedule.hpp>
#include <dcd/singleton.hpp>
#include <dcd/system.hpp>
#include <dcd/time.hpp>

namespace dcdsystem {

   using dcd::asset;
   using dcd::block_timestamp;
   using dcd::check;
   using dcd::const_mem_fun;
   using dcd::datastream;
   using dcd::indexed_by;
   using dcd::name;
   using dcd::same_payer;
   using dcd::symbol;
   using dcd::symbol_code;
   using dcd::time_point;
   using dcd::time_point_sec;
   using dcd::unsigned_int;



    // Defines `oracle_info` structure to be stored in `oracle_info` table
    struct [[dcd::table, dcd::contract("dcd.system")]] oracle_info {
        name                                                     owner;
        time_point                                               fee_rate_time;
        double                                                   fee_rate = 0.0;
        
        uint64_t primary_key()const { return owner.value; }

        oracle_info(){};

        // explicit serialization macro is not necessary, used here only to improve compilation time    
        EOSLIB_SERIALIZE( oracle_info, (owner)(fee_rate_time)(fee_rate) )


    };

   typedef dcd::multi_index< "oracles"_n, oracle_info > oracle_table;
   
   struct action_fee_prop {
         name account;
         name action;
         asset fee;     
         EOSLIB_SERIALIZE( action_fee_prop, (account)(action)(fee) )
      };
 
   struct action_fee_prop_core{
      name                                                     owner;
      std::vector <action_fee_prop>                            fee_prop_list;
      bool                                                     is_active;
      time_point                                               propose_rate_time;
   };

   struct [[dcd::table, dcd::contract("dcd.system")]] actions_fee_proposals {
      name                                                     owner;
      std::vector <action_fee_prop>                            fee_prop_list;
      time_point                                               propose_rate_time;
      bool                                                     is_active;

      uint64_t primary_key()const { return owner.value; }
      uint64_t by_time()const { return propose_rate_time.elapsed.count(); }
      std::set<name>                                            voted;
      EOSLIB_SERIALIZE( actions_fee_proposals, (owner)(fee_prop_list)(propose_rate_time)(is_active) )
   };

   typedef dcd::multi_index< "feeproposals"_n, actions_fee_proposals,
                                indexed_by<"bytime"_n, const_mem_fun<actions_fee_proposals, uint64_t, &actions_fee_proposals::by_time> >
                                > actions_fee_proposals_table;
}