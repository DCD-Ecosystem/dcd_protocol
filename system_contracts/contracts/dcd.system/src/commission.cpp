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
       auto orcl = _oracles.find( oracle.value );
       const auto ct = dcd::current_time_point();


       if ( orcl == _oracles.end() ) {
         _oracles.emplace(oracle, [&](oracle_info& info){
            info.owner = oracle;
            info.fee_rate = 0.0;
            info.fee_rate_time = ct;
         });
       }
      else{
         print("User already added as an oracle");
         return;
        }

      //force oracle rate recalculation
      calculate_new_oracle_rate();
    
   }

   void system_contract::unregoracle( const name& oracle ) {
       dcd::print("UnReg Oracle called");
        auto orcl = _oracles.find( oracle.value );

       check(orcl != _oracles.end(),  "User not added as an oracle" );
        _oracles.erase(orcl);
        calculate_new_oracle_rate();

   }

   void system_contract::setrate( const name& oracle, const double fee_rate ) {
      dcd::print("Set rate for oracle called");

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


      native::setfeeforce_action _setfeerate_action {"dcd"_n, {get_self(), "active"_n}};
      _setfeerate_action.send(rate_to_set);
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

         native::setfeeforce_action _setfeerate_action {"dcd"_n, {get_self(), "active"_n}};
         _setfeerate_action.send(median);
      }
   }

   void system_contract::propactfee( const name& producer, std::vector <action_fee_prop> prop_fees ) {
      dcd::print("setting fee");      
      auto prod = _fee_proposals.find( producer.value );
      const auto ct = dcd::current_time_point();

      if ( prod != _fee_proposals.end() ) {
         _fee_proposals.modify( prod, producer, [&]( actions_fee_proposals& info ){
            info.owner              = producer;
            info.is_active          = true;
            info.propose_rate_time  = ct;
            if(info.fee_prop_list.size() > 0)
               info.fee_prop_list.clear();
            copy(prop_fees.begin(), prop_fees.end(), back_inserter(info.fee_prop_list));
            
         });
      } else {
         _fee_proposals.emplace( producer, [&]( actions_fee_proposals& info ){
            info.owner              = producer;
            info.is_active          = true;
            info.propose_rate_time  = ct;
            copy(prop_fees.begin(), prop_fees.end(), back_inserter(info.fee_prop_list));
         });
   
      }
   } 

   void system_contract::votefeeprop(const name& producer, unsigned int accept)
   {  
      dcd::print("Voting for the fee called");       


   }

   void system_contract::listfeeprop() {
   
      dcd::print("listing proposed fee");
      auto ct = to_string(dcd::current_time_point().elapsed.count());
      print(ct);
   
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


   bool system_contract::has_voted_for_act_fee(const name& producer, const actions_fee_proposals& actions_fee_prop)
   {

      
   }


}