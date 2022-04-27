/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/transaction_fee_manager.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/chain/contract_table_objects.hpp>
#include <iostream>

namespace eosio { namespace chain {

   transaction_fee_manager::transaction_fee_manager(){
      init_native_fee(config::system_account_name, name{"defaultfee"} , asset::from_string("0.0000 USD"));
   }

   void copy_inline_row_my(const chain::key_value_object& obj, vector<char>& data) {
      data.resize( obj.value.size() );
      memcpy( data.data(), obj.value.data(), obj.value.size() );
   }

   void  transaction_fee_manager::get_new_rate_params( const controller& ctl ) {
      contract_cur_rate = ctl.active_rate().rate;
   }



   asset transaction_fee_manager::get_core_from_usd(const asset& usd) const {
      uint64_t core_precision = symbol(CORE_SYMBOL).precision();
      uint64_t usd_precision = usd.precision();
      asset a = asset( int64_t( (usd.get_amount() * core_precision) / (usd_precision * contract_cur_rate) ), symbol(CORE_SYMBOL));
      return a;
   }

   asset transaction_fee_manager::get_required_fee( const controller& ctl, const transaction& trx ) const {
      auto fee = asset(0);
      for (const auto& act : trx.actions ) {
         fee += get_required_fee(ctl, act);
      }
      return fee;
   }

   asset transaction_fee_manager::get_required_fee( const controller& ctl, const account_name& account, const action_name& act ) const {
      const auto &db = ctl.db();
      asset res_fee = asset(0);

      // first check if changed fee
      try{
         const auto fee_in_db = db.find<action_fee_object, by_action_name>(
               boost::make_tuple(account, act));
         if( fee_in_db != nullptr ) {
            res_fee += get_core_from_usd(fee_in_db->fee);
            return res_fee;
         }
      } catch (fc::exception &exp){
         elog("catch exp ${e}", ("e", exp.what()));
      } catch (...){
         elog("catch unknown exp in get_required_fee");
      }

      const auto native_fee = get_native_fee(account, act);
      if (native_fee != asset(0)) {
         return native_fee;
      }

      const auto default_fee_in_db = db.find<action_fee_object, by_action_name>(
         boost::make_tuple(config::system_account_name, name{"defaultfee"}));
         if( default_fee_in_db != nullptr ) {
            res_fee += get_core_from_usd(default_fee_in_db->fee);
            return res_fee;
         }
      return get_native_fee(config::system_account_name, name{"defaultfee"});
   }

   asset transaction_fee_manager::get_required_fee( const controller& ctl, const action& act ) const {
      return get_required_fee( ctl, act.account, act.name );
   }


   asset transaction_fee_manager::get_base_rate_asset(const controller& ctl) const {
      return base_rate_asset;
   }

   action_fee_info transaction_fee_manager::get_action_fee(const controller& ctl, const account_name& account, const action_name& act ) const {
      
       action_fee_info info;
       info.cur_rate = contract_cur_rate;

       const auto &db = ctl.db();
       //first check if changed fee
       try{
          const auto fee_in_db = db.find<action_fee_object, by_action_name>(
                boost::make_tuple(account, act));
          if( fee_in_db != nullptr ) {
             info.core_fee = get_core_from_usd(fee_in_db->fee);
             info.usd_fee = fee_in_db->fee;
             return info;
          }
       } catch (fc::exception &exp){
          elog("catch exp ${e}", ("e", exp.what()));
       } catch (...){
          elog("catch unknown exp in get_required_fee");
       }
       const auto default_fee_in_db = db.find<action_fee_object, by_action_name>(
          boost::make_tuple(config::system_account_name, name{"defaultfee"}));
          if( default_fee_in_db != nullptr ) {
             info.core_fee = get_core_from_usd(default_fee_in_db->fee);
             info.usd_fee = default_fee_in_db->fee;
             return info;
          }

       info.core_fee = get_native_fee(config::system_account_name, name{"defaultfee"});
       const auto itr = fee_map.find(std::make_pair(config::system_account_name, name{"defaultfee"}));
       info.usd_fee = itr->second;
       return info;
   }

   fee_rate_info transaction_fee_manager::get_fee_rate(const controller& ctl) const {

      fee_rate_info info;
      info.base_rate_asset = base_rate_asset;
      info.new_rate_period = contract_new_rate_period;
      info.out_of_date_time = contract_out_of_date_time;
      info.prev_rate = contract_prev_rate;
      info.cur_rate = contract_cur_rate;
      info.cur_rate_time = contract_cur_rate_time;
      info.cur_rate_producers = contract_cur_rate_producers;
      return info;
   }


}}