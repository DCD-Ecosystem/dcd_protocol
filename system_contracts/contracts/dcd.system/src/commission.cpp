#include <dcd/crypto.hpp>
#include <dcd/datastream.hpp>
#include <dcd/dcd.hpp>
#include <dcd/multi_index.hpp>
#include <dcd/serialize.hpp>
#include <dcd/singleton.hpp>
#include <dcd.system/native.hpp>
#include <dcd/print.hpp>


#include <dcd.system/dcd.system.hpp>
#include <dcd.system/commission.hpp>
#include <dcd.token/dcd.token.hpp>

#include <type_traits>
#include <limits>
#include <set>
#include <algorithm>
#include <cmath>
#include <string>


namespace dcdsystem {

   using dcd::const_mem_fun;
   using dcd::current_time_point;
   using dcd::indexed_by;
   using dcd::microseconds;
   using dcd::singleton;
   using dcd::print;
   using namespace std;


   void system_contract::regoracle( const name& oracle ) {
       dcd::print("Reg Oracle called");
       require_auth( oracle );

       auto orcl = _oracles.find( oracle.value );

       const auto ct = dcd::current_time_point();
       check( orcl == _oracles.end(),"User already added as an oracle" );

      _oracles.emplace(oracle, [&](oracle_info& info){
            info.owner = oracle;
            info.fee_rate = 0.0;
            info.fee_rate_time = ct;
         });
      //force oracle rate recalculation
      calculate_new_oracle_rate();
   }

   void system_contract::unregoracle( const name& oracle ) {
      dcd::print("UnReg Oracle called");
      require_auth( oracle );
      auto orcl = _oracles.find( oracle.value );

      check(orcl != _oracles.end(),  "User not added as an oracle" );
      _oracles.erase(orcl);
      calculate_new_oracle_rate();

   }

   void system_contract::setrateorcl( const name& oracle, const double fee_rate ) {
      dcd::print("Set rate for oracle called");
      require_auth( oracle );

      auto orcl = _oracles.find( oracle.value );
      check(orcl != _oracles.end(),  "User not added as an oracle" );
      check(fee_rate > 0, "Rate needs to be > than 0.0");
      const auto ct = dcd::current_time_point();

      _oracles.modify( orcl, oracle, [&] ( oracle_info& or_table ) {
         or_table.fee_rate = fee_rate;
         or_table.fee_rate_time = ct;
      });

      calculate_new_oracle_rate();    
      dcd::print("Set rate for oracle called");
   }

   void system_contract::calculate_new_oracle_rate()
   {     
      double fee_rate = 0.0;
      double default_rate = 1.0;
      double rate_to_set = default_rate;

      vector<int> eligible_rates;
      for(auto orcl : _oracles) {
         if(orcl.fee_rate > 0)
            eligible_rates.push_back(orcl.fee_rate);
      } 

      size_t size = eligible_rates.size();
      if (size > 0)
      {
         sort(eligible_rates.begin(), eligible_rates.end());
         if (size % 2 == 0)
         {
            rate_to_set = (eligible_rates[size / 2 - 1] + eligible_rates[size / 2]) / 2;
         }
         else 
         {
            rate_to_set = eligible_rates[size / 2];
         }
      }
      else 
         rate_to_set = default_rate;

      if(rate_to_set != _gstate3.last_oracle_rate)
      {
            native::setrate_action _setfeerate_action {"dcd"_n, {get_self(), "active"_n}};
            _setfeerate_action.send(rate_to_set);
            _gstate3.last_oracle_rate = rate_to_set;
      }
   } 
   
   void system_contract::update_current_rate( const block_timestamp& block_time )
   {
      calculate_new_oracle_rate();
      if(block_time.slot < (_gstate.cur_rate_time.slot + _gstate.new_rate_period*2)) {
         return;
      }

   } 

   void system_contract::calculate_new_rate( const block_timestamp& block_time )
   {
      if(block_time.slot < (_gstate.cur_rate_time.slot + _gstate.new_rate_period*2)) {
         return;
      }

      std::vector<producers_rate_info> new_producers;
      std::vector<double> rates;

      std::vector<name> active_producers = dcd::get_active_producers();
      for(auto producer: active_producers) {
         auto prod = _producers.find( producer.value );
         if( prod == _producers.end()) // dcd producer
            return;

         if (prod->fee_rate_time == time_point()) {
            continue;
         }
         
         if (prod->fee_rate_time.sec_since_epoch() + _gstate.out_of_date_time < block_time.to_time_point().sec_since_epoch()) {
            continue;
         }

         producers_rate_info info;
         info.name = producer;
         info.rate_time = prod->fee_rate_time;
         info.rate = prod->fee_rate;
         new_producers.push_back(info);
         rates.push_back(prod->fee_rate);
      }

      uint32_t rates_size = rates.size();

      _gstate.cur_rate_time = block_time;
      if(rates_size) {
         std::sort(rates.begin(), rates.end());
         double median;

         if(rates_size%2)
            median = rates[rates_size/2];
         else 
            median = (rates[rates_size/2 - 1] + rates[rates_size/2])/2;

         _gstate.cur_rate_producers = new_producers;
         _gstate.prev_rate = _gstate.cur_rate;
         _gstate.cur_rate = median;

         native::setrate_action _setfeerate_action {"dcd"_n, {get_self(), "active"_n}};
         _setfeerate_action.send(median);
      }
   }

   void system_contract::propactfee( const name& producer, std::vector <action_fee_prop> prop_fees ) {

      require_auth( producer );   
      const auto& prod_ver = _producers.find( producer.value);
      check(prod_ver != _producers.end(),"Account not found in producers");
      check(prop_fees.size() <= 30, "More than 30 actions in the suggestion");

      auto prod = _fee_proposals.find( producer.value );
      const auto ct = dcd::current_time_point();
      const auto expires = time_point(ct.time_since_epoch() + microseconds(fee_vote_expiration_limit * 1000000));
      if ( prod != _fee_proposals.end() ) {
         _fee_proposals.modify( prod, producer, [&]( actions_fee_proposals& info ){
            info.owner              = producer;
            info.propose_fee_time   = ct;
            info.expires_at         = expires;
            if(info.fee_prop_list.size() > 0)
               info.fee_prop_list.clear();
            for (auto prop : prop_fees)
                info.fee_prop_list.push_back(prop);                    
            info.voted_approve       = 1;
            info.voted_disapprove    = 0;
            info.is_active          = true;
            info.voted.clear();
            info.voted.insert(producer);       
         });
      } else {
         _fee_proposals.emplace( producer, [&]( actions_fee_proposals& info ){
            info.owner              = producer;
            info.propose_fee_time   = ct;
            info.expires_at         = expires;
            for (auto prop : prop_fees)
                info.fee_prop_list.push_back(prop);                    
            info.voted_approve       = 1;
            info.voted_disapprove    = 0;
            info.is_active          = true;
            info.voted.insert(producer);      
         });    
      }


      std::vector<name> accounts;
      std::vector<name> actions; 
      std::vector<asset> fees;

      for(auto p : prop_fees)
      {
         accounts.push_back(p.account);
         actions.push_back(p.action);
         fees.push_back(p.fee);
      }
      native::procfeeprop_action _procfeeprop_action {"dcd"_n, {get_self(), "active"_n}};
      _procfeeprop_action.send(producer, accounts, actions, fees, ct, expires);  
   } 

   void system_contract::votefeeprop(const name& producer, const name& prod_suggestion , unsigned int accept)
   {  
      require_auth( producer );   
      const auto& prod_ver = _producers.find( producer.value);
      check(prod_ver != _producers.end(),"Account not found in producers");

      auto prod_suggested = _fee_proposals.find( prod_suggestion.value );
      check(prod_suggested !=  _fee_proposals.end(), "No suggestions from producer" );  

      _fee_proposals.modify( prod_suggested, prod_suggestion, [&]( actions_fee_proposals& info ){
         check(info.voted.find(producer) == info.voted.end(),"Producer already voted");
         if(accept)
               info.voted_approve++;
         else
               info.voted_disapprove++;
         
         info.voted.insert(producer);
      
      });    
      check_action_proposal_table();       

   }
      
   void system_contract::setrateprod( const name& producer, const double fee_rate ) {
      require_auth( producer );

      check( fee_rate > 0, "fee_rate must be positive" );
      const auto prod = _producers.find( producer.value );
      check( prod != _producers.end(), "producer not found" );

      _producers.modify( prod, same_payer, [&]( producer_info& info ){
         info.fee_rate = fee_rate;
         info.fee_rate_time = current_time_point();
      });
      
   }

   void system_contract::update_action_fees(std::vector <native::action_fee_prop> prop_list)
   {
      
      native::setfee_action _setfee_action {"dcd"_n, {get_self(), "active"_n}};
      for(auto prop : prop_list)
      {        
         _setfee_action.send(prop.account,prop.action, prop.fee);
      }
   }

   void system_contract::check_action_proposal_table()
   {
      time_point curr_time = dcd::current_time_point();
      
      std::set<name> props_to_delete;
      std::map<microseconds,name> props_to_apply; 
      native::rmfeeprop_action _rmfeeprop_action {"dcd"_n, {get_self(), "active"_n}};

      update_producers_count();
      auto prod_count = _gstate3.producers_count; 
      unsigned int votes_to_pass = floor ( double(prod_count) * vote_percent_to_succeed) + 1;
      unsigned int votes_to_fail = floor( double(prod_count) * vote_percent_to_fail );

      for(auto prop : _fee_proposals)
      {
         //Check if voting is concluded
         if(prod_count) {
            if  ( prop.voted_approve >= votes_to_pass) {

               name s {prop.owner.value};
               auto prop_to_approve = _fee_proposals_fin.find(s.value);
               time_point t_to_apply = time_point(curr_time.time_since_epoch() + microseconds(fee_approved_application_delay * 1000000));
               
               if ( prop_to_approve != _fee_proposals_fin.end() ) {
                  _fee_proposals_fin.modify( prop_to_approve, prop.owner, [&]( fee_proposals_finalized& info ){
                  info.owner              = prop.owner;
                  info.fee_prop_list.clear();
                  info.fee_prop_list      = prop.fee_prop_list;
                  info.approved_time      = t_to_apply;
                  });
               } else {
                   _fee_proposals_fin.emplace( prop.owner, [&]( fee_proposals_finalized& info ){
                  info.owner              = prop.owner;
                  info.fee_prop_list      = prop.fee_prop_list;
                  info.approved_time      = t_to_apply;
                   });    
               }

               props_to_delete.insert(prop.owner);
            } else if ( prop.voted_disapprove >= votes_to_fail ) {  
                props_to_delete.insert(prop.owner);
            }
            
         }

          //Check if any tables need to be deleted
         if ( curr_time >= prop.expires_at)
            props_to_delete.insert(prop.owner);

      }        
      //Delete timed and apporoved fee lists
      for (auto to_del : props_to_delete)
      { 
         auto itr = _fee_proposals.find( to_del.value );
         _fee_proposals.erase(itr);
         _rmfeeprop_action.send(to_del);  
      }

      //Check the approved fees table, apply after delay and delete
      std::set<name> props_appl_to_delete;

      for (auto to_appl : _fee_proposals_fin)
      {
         if (curr_time >= to_appl.approved_time)
         {  
            update_action_fees(to_appl.fee_prop_list);
            props_appl_to_delete.insert(to_appl.owner);
         }
      }

      for (auto to_del : props_appl_to_delete)
      { 
         auto itr = _fee_proposals_fin.find( to_del.value );
         _fee_proposals_fin.erase(itr);
      }
   }

   void system_contract::update_oracles_rate_state() {
      time_point curr_time = dcd::current_time_point();
      if ( ( curr_time.sec_since_epoch() -_gstate3.oracles_last_updated.sec_since_epoch() ) >= oracles_update_period_seconds)
      {
         calculate_new_oracle_rate();
         _gstate3.oracles_last_updated = curr_time;
      }
   }
   
   void system_contract::update_fee_vote_state() {
      
      time_point curr_time = dcd::current_time_point();
      if ( (curr_time.sec_since_epoch() -  _gstate3.commision_last_update.sec_since_epoch() ) >= proposed_fee_update_period_seconds)
      {
         check_action_proposal_table();
         _gstate3.commision_last_update = curr_time;
      }
   }

}