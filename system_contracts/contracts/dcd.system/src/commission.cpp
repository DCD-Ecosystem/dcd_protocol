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
       require_auth( oracle );

       auto orcl = _oracles.find( oracle.value );

       const auto ct = dcd::current_time_point();
       check( orcl == _oracles.end(),"User already added as an oracle" );

      _oracles.emplace(oracle, [&](oracle_info& info){
            info.owner = oracle;
            info.fee_rate = 0.0;
            info.fee_rate_time = ct;
         });
   }

   void system_contract::unregoracle( const name& oracle ) {
      require_auth( oracle );
      auto orcl = _oracles.find( oracle.value );

      check(orcl != _oracles.end(),  "User not added as an oracle" );
      _oracles.erase(orcl);
   }

   void system_contract::setrateorcl( const name& oracle, const double fee_rate ) {
      require_auth( oracle );

      auto orcl = _oracles.find( oracle.value );
      check(orcl != _oracles.end(),  "User not added as an oracle" );
      check(fee_rate > 0, "Rate needs to be > than 0.0");
      const auto ct = dcd::current_time_point();

      _oracles.modify( orcl, oracle, [&] ( oracle_info& or_table ) {
         or_table.fee_rate = fee_rate;
         or_table.fee_rate_time = ct;
      });
   }

   void system_contract::calculate_new_oracle_rate() {     

      double default_rate = 1.0;
      double rate_to_set = default_rate;

      vector<double> eligible_rates;
      for(auto orcl : _oracles) {
         if(orcl.fee_rate > 0.0)
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
         const auto ct = dcd::current_time_point(); 
         native::setrate_action _setfeerate_action {"dcd"_n, {get_self(), "active"_n}};
         _setfeerate_action.send(rate_to_set, ct);
         _gstate3.last_oracle_rate = rate_to_set;
      }
   } 
   
   void system_contract::propactfee( const name& producer, std::vector <action_fee_prop> prop_fees ) {

      require_auth( producer );   
      const auto& prod_ver = _producers.find( producer.value);
      check(prod_ver != _producers.end(),"Account not found in producers");
      check(prop_fees.size() <= 30, "More than 30 actions in the suggestion");

      const auto ct = dcd::current_time_point();
      const auto expires = time_point( microseconds( (ct.sec_since_epoch() + fee_vote_expiration_limit) * 1000000));
      auto prod = _fee_proposals.find( producer.value );
      if ( prod != _fee_proposals.end() ) {
         _fee_proposals.modify( prod, producer, [&]( actions_fee_proposals& info ){
            info.owner              = producer;
            info.proposed_at        = ct;
            info.expires_at         = expires;
            if(info.fee_prop_list.size() > 0)
               info.fee_prop_list.clear();
            for (auto prop : prop_fees)
                info.fee_prop_list.push_back(prop);                    
            info.voted_approve         = 1;
            info.voted_disapprove      = 0;
            info.is_active             = true;
            info.voted.clear();
            info.voted.insert(producer);       
         });
      } else {
         _fee_proposals.emplace( producer, [&]( actions_fee_proposals& info ){
            info.owner              = producer;
            info.proposed_at        = ct;
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
         if(accept > 0 )
               info.voted_approve++;
         else
               info.voted_disapprove++;
         
         info.voted.insert(producer);
      });
   }
      
   void system_contract::check_action_proposal_table(time_point ct) {

         std::set<name> props_to_delete;
         update_producers_count();
         auto prod_count = _gstate3.producers_count; 
         unsigned int votes_to_pass = floor ( double(prod_count) * vote_percent_to_succeed) + 1;
         unsigned int votes_to_fail = floor( double(prod_count) * vote_percent_to_fail );

         for(auto prop : _fee_proposals)
         {
            //Check if voting is concluded delete the proposition if yes
            if(prod_count) {
               if  ( prop.voted_approve >= votes_to_pass) {
                  props_to_delete.insert(prop.owner);
                  time_point to_be_applied = time_point( microseconds( (ct.sec_since_epoch() + fee_approved_application_delay) * 1000000));

                  for (auto p : prop.fee_prop_list ) {
                     auto acc_act = name {p.account.value + p.action.value};
                     auto pr = _fee_changes.find(acc_act.value);
                     if ( pr != _fee_changes.end() ) {
                        _fee_changes.modify( pr,acc_act, [&]( fee_changes& info ) {
                           if (prop.proposed_at > info.proposed_at)
                              info.fee = p.fee;
                              info.proposed_at = prop.proposed_at;
                              info.to_be_applied_at = to_be_applied;
                           });
                     } else {
                        _fee_changes.emplace(acc_act, [&]( fee_changes& info ){
                              info.account_action = acc_act;
                              info.account = p.account;
                              info.action = p.action;
                              info.fee = p.fee;
                              info.proposed_at = prop.proposed_at;
                              info.to_be_applied_at = to_be_applied;
                        });    
                     }  
                  }               

               }
               if ( prop.voted_disapprove >= votes_to_fail) {
                  props_to_delete.insert(prop.owner);
               }               
            
            }

            if ( ct >= prop.expires_at)
               props_to_delete.insert(prop.owner);
         }

     
         //Delete timed and apporoved fee lists
         for (auto to_del : props_to_delete)
         { 
            auto itr = _fee_proposals.find( to_del.value );
            _fee_proposals.erase(itr);
         }

   }

   void system_contract::update_oracles_rate_state(time_point ct) {
      if ( ( ct.sec_since_epoch() - _gstate3.oracles_last_updated.sec_since_epoch() ) >= oracles_update_period_seconds)
      {
         calculate_new_oracle_rate();
         _gstate3.oracles_last_updated = ct;
      }
   }

   void system_contract::update_fee_vote_state(time_point ct) {
      
      if ( (ct.sec_since_epoch() -  _gstate3.commision_last_update.sec_since_epoch() ) >= proposed_fee_update_period_seconds)
      {
         check_action_proposal_table(ct);
         _gstate3.commision_last_update = ct;
      }
   }

   void system_contract::update_producers_count() {
      uint32_t count = 0;
      for (const auto prod : _producers)
         ++count;
      _gstate3.producers_count = count;
   }
   
   void system_contract::apply_pending_fee_changes(time_point ct) {  
      if ( (ct.sec_since_epoch() - _gstate3.fee_changes_last_update.sec_since_epoch() ) >= applied_fees_update_period_seconds)
      {
         std::set<name> to_del;
         native::setfee_action _setfee_action {"dcd"_n, {get_self(), "active"_n}};

         for (auto fa : _fee_changes) {
            if(ct >= fa.to_be_applied_at)
            {
               _setfee_action.send(fa.account,fa.action,fa.fee);
               to_del.insert(fa.account_action);
            }
         }
         for (auto n : to_del) {
            auto itr = _fee_changes.find( n.value );
            _fee_changes.erase(itr);
         } 
         _gstate3.fee_changes_last_update = ct;
      }
   }

   void system_contract::on_block_updates() {
      time_point ct = dcd::current_time_point();
      apply_pending_fee_changes(ct);
      update_oracles_rate_state(ct);
      update_fee_vote_state(ct);
   } 
}