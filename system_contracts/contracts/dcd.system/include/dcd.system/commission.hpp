#pragma once

#include <dcd/asset.hpp>
#include <dcd/binary_extension.hpp>
#include <dcd/privileged.hpp>
#include <dcd/producer_schedule.hpp>
#include <dcd/singleton.hpp>
#include <dcd/system.hpp>
#include <dcd/time.hpp>
#include <dcd.system/native.hpp>

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
        DCDLIB_SERIALIZE( oracle_info, (owner)(fee_rate_time)(fee_rate) )
    };

   typedef dcd::multi_index< "oracles"_n, oracle_info > oracle_table;
   
  
   struct action_fee_prop {
         name account;
         name action;
         asset fee;
         DCDLIB_SERIALIZE(action_fee_prop, (account)(action)(fee))
   };

    // Sugeested fee proposals. Stores information about proposed fees
   // - `owner` the producer sending the fee propositions
   // - `proposed_at` time stamp at which the proposal 
   // - `fee_prop_list` the list of submited fees for actions

   struct [[dcd::table, dcd::contract("dcd.system")]] actions_fee_proposals {
      name                                                     owner;
      time_point                                               proposed_at;
      time_point                                               expires_at; 
      std::vector <action_fee_prop>                            fee_prop_list;
      uint32_t                                                 voted_approve = 0;
      uint32_t                                                 voted_disapprove = 1;
      bool                                                     is_active = false;
      std::set<name>                                           voted;
      uint64_t primary_key()const { return owner.value; }
      uint64_t by_time()const { return proposed_at.elapsed.count(); }

      DCDLIB_SERIALIZE( actions_fee_proposals, (owner)(proposed_at)(expires_at)(fee_prop_list)(is_active)(voted)(voted_approve)(voted_disapprove))
   };

   typedef dcd::multi_index< "feeproposals"_n, actions_fee_proposals,
                                 indexed_by<"bytime"_n, const_mem_fun<actions_fee_proposals, uint64_t, &actions_fee_proposals::by_time> >
                                 > actions_fee_proposals_table;

   // Apporved fee proposals. Stores information about proposed fees that have been approved until application time
   // - `owner` the producer sending the fee propositions
   // - `application_time` time stamp after which the proposals need to be applied
   // - `fee_prop_list` the list of submited fees for actions


   struct [[dcd::table, dcd::contract("dcd.system")]] fee_proposals_finalized {
      name                                                     owner;
      time_point                                               application_time;
      std::vector <action_fee_prop>                            fee_prop_list;
      time_point                                               proposed_at;
      uint64_t primary_key()const { return owner.value; }

      DCDLIB_SERIALIZE( fee_proposals_finalized, (owner)(application_time)(fee_prop_list)(proposed_at))      
   };

   typedef dcd::multi_index< "finproposals"_n, fee_proposals_finalized > fee_proposals_finalized_table;
   
   struct [[dcd::table, dcd::contract("dcd.system")]] fee_changes {
      
      name                                                     account_action;
      name                                                     account;
      name                                                     action;
      asset                                                    fee;
      time_point                                               proposed_at;
      time_point                                               to_be_applied_at;
      uint64_t primary_key()const { return (account_action.value); }
      DCDLIB_SERIALIZE( fee_changes, (account_action)(account)(action)(fee)(proposed_at))      
   };

   typedef dcd::multi_index< "feechanges"_n, fee_changes > fee_changes_table;
}