#pragma once

#include <dcd/chain/authority.hpp>
#include <dcd/chain/types.hpp>

namespace dcd { namespace chain {

using action_name    = dcd::chain::action_name;

struct newaccount {
   account_name                     creator;
   account_name                     name;
   authority                        owner;
   authority                        active;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "newaccount"_n;
   }
};

struct setcode {
   account_name                     account;
   uint8_t                          vmtype = 0;
   uint8_t                          vmversion = 0;
   bytes                            code;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "setcode"_n;
   }
};

struct setabi {
   account_name                     account;
   bytes                            abi;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "setabi"_n;
   }
};

struct setfee{
   account_name  account;
   action_name   action;
   asset         fee;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "setfee"_n;
   }
};


struct setrate{
   double        new_rate;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "setrate"_n;
   }


};

struct updateauth {
   account_name                      account;
   permission_name                   permission;
   permission_name                   parent;
   authority                         auth;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "updateauth"_n;
   }
};

struct deleteauth {
   deleteauth() = default;
   deleteauth(const account_name& account, const permission_name& permission)
   :account(account), permission(permission)
   {}

   account_name                      account;
   permission_name                   permission;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "deleteauth"_n;
   }
};

struct linkauth {
   linkauth() = default;
   linkauth(const account_name& account, const account_name& code, const action_name& type, const permission_name& requirement)
   :account(account), code(code), type(type), requirement(requirement)
   {}

   account_name                      account;
   account_name                      code;
   action_name                       type;
   permission_name                   requirement;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "linkauth"_n;
   }
};

struct unlinkauth {
   unlinkauth() = default;
   unlinkauth(const account_name& account, const account_name& code, const action_name& type)
   :account(account), code(code), type(type)
   {}

   account_name                      account;
   account_name                      code;
   action_name                       type;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "unlinkauth"_n;
   }
};

struct canceldelay {
   permission_level      canceling_auth;
   transaction_id_type   trx_id;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "canceldelay"_n;
   }
};

struct onerror {
   uint128_t      sender_id;
   bytes          sent_trx;

   onerror( uint128_t sid, const char* data, size_t len )
   :sender_id(sid),sent_trx(data,data+len){}

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "onerror"_n;
   }



};

struct rmfeeprop{
   name        owner;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "rmfeeprop"_n;
   }
};

struct action_fee_prop {
         name account;
         name action;
         asset fee;     
};


struct procfeeprop{
   name        owner;

   std::vector<name> accounts;
   std::vector<name> actions;
   std::vector<asset> fees;

   time_point proposed_at;
   time_point expires_at;

   static account_name get_account() {
      return config::system_account_name;
   }

   static action_name get_name() {
      return "procfeeprop"_n;
   }
};


} } /// namespace dcd::chain

FC_REFLECT( dcd::chain::newaccount                       , (creator)(name)(owner)(active) )
FC_REFLECT( dcd::chain::setcode                          , (account)(vmtype)(vmversion)(code) )
FC_REFLECT( dcd::chain::setabi                           , (account)(abi) )
FC_REFLECT( dcd::chain::updateauth                       , (account)(permission)(parent)(auth) )
FC_REFLECT( dcd::chain::deleteauth                       , (account)(permission) )
FC_REFLECT( dcd::chain::linkauth                         , (account)(code)(type)(requirement) )
FC_REFLECT( dcd::chain::unlinkauth                       , (account)(code)(type) )
FC_REFLECT( dcd::chain::canceldelay                      , (canceling_auth)(trx_id) )
FC_REFLECT( dcd::chain::onerror                          , (sender_id)(sent_trx) )
FC_REFLECT( dcd::chain::setfee                           , (account)(action)(fee) )
FC_REFLECT( dcd::chain::setrate                          , (new_rate) )
FC_REFLECT( dcd::chain::procfeeprop                      , (owner)(accounts)(actions)(fees)(proposed_at)(expires_at))
FC_REFLECT( dcd::chain::rmfeeprop                        , (owner) )
FC_REFLECT(dcd::chain::action_fee_prop                   , (account)(action)(fee))