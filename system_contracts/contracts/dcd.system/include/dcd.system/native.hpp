#pragma once

#include <dcd/action.hpp>
#include <dcd/contract.hpp>
#include <dcd/crypto.hpp>
#include <dcd/fixed_bytes.hpp>
#include <dcd/ignore.hpp>
#include <dcd/print.hpp>
#include <dcd/privileged.hpp>
#include <dcd/producer_schedule.hpp>
#include <dcd/asset.hpp>


namespace dcdsystem {

   using dcd::checksum256;
   using dcd::ignore;
   using dcd::name;
   using dcd::permission_level;
   using dcd::public_key;
   using dcd::asset;

   /**
    * A weighted permission.
    *
    * Defines a weighted permission, that is a permission which has a weight associated.
    * A permission is defined by an account name plus a permission name.
    */
   struct permission_level_weight {
      permission_level  permission;
      uint16_t          weight;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( permission_level_weight, (permission)(weight) )
   };

   /**
    * Weighted key.
    *
    * A weighted key is defined by a public key and an associated weight.
    */
   struct key_weight {
      dcd::public_key  key;
      uint16_t           weight;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( key_weight, (key)(weight) )
   };

   /**
    * Wait weight.
    *
    * A wait weight is defined by a number of seconds to wait for and a weight.
    */
   struct wait_weight {
      uint32_t           wait_sec;
      uint16_t           weight;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( wait_weight, (wait_sec)(weight) )
   };

   /**
    * Blockchain authority.
    *
    * An authority is defined by:
    * - a vector of key_weights (a key_weight is a public key plus a wieght),
    * - a vector of permission_level_weights, (a permission_level is an account name plus a permission name)
    * - a vector of wait_weights (a wait_weight is defined by a number of seconds to wait and a weight)
    * - a threshold value
    */
   struct authority {
      uint32_t                              threshold = 0;
      std::vector<key_weight>               keys;
      std::vector<permission_level_weight>  accounts;
      std::vector<wait_weight>              waits;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( authority, (threshold)(keys)(accounts)(waits) )
   };

   struct rate_schedule {
      /**
       * Version number of the schedule. It is sequentially incrementing version number
       */
      uint32_t                     version;

      /**
       *The rate for the schedule
       */
      double                       rate;
   };


   /**
    * Blockchain block header.
    *
    * A block header is defined by:
    * - a timestamp,
    * - the producer that created it,
    * - a confirmed flag default as zero,
    * - a link to previous block,
    * - a link to the transaction merkel root,
    * - a link to action root,
    * - a schedule version,
    * - and a producers' schedule.
    */
   struct block_header {
      uint32_t                                  timestamp;
      name                                      producer;
      uint16_t                                  confirmed = 0;
      checksum256                               previous;
      checksum256                               transaction_mroot;
      checksum256                               action_mroot;
      uint32_t                                  schedule_version = 0;
      uint32_t                                  rate_version = 0;
      std::optional<rate_schedule>              new_rate;
      std::optional<dcd::producer_schedule>     new_producers;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE(block_header, (timestamp)(producer)(confirmed)(previous)(transaction_mroot)(action_mroot)
                                     (schedule_version)(new_producers)(rate_version)(new_rate))
   };

   /**
    * abi_hash is the structure underlying the abihash table and consists of:
    * - `owner`: the account owner of the contract's abi
    * - `hash`: is the sha256 hash of the abi/binary
    */
   struct [[dcd::table("abihash"), dcd::contract("dcd.system")]] abi_hash {
      name              owner;
      checksum256       hash;
      uint64_t primary_key()const { return owner.value; }

      EOSLIB_SERIALIZE( abi_hash, (owner)(hash) )
   };

   // Method parameters commented out to prevent generation of code that parses input data.
   /**
    * The DCD core `native` contract that governs authorization and contracts' abi.
    */
   class [[dcd::contract("dcd.system")]] native : public dcd::contract {
      public:

         using dcd::contract::contract;

         /**
          * These actions map one-on-one with the ones defined in core layer of DCD, that's where their implementation
          * actually is done.
          * They are present here only so they can show up in the abi file and thus user can send them
          * to this contract, but they have no specific implementation at this contract level,
          * they will execute the implementation at the core layer and nothing else.
          */
         /**
          * New account action is called after a new account is created. This code enforces resource-limits rules
          * for new accounts as well as new account naming conventions.
          *
          * 1. accounts cannot contain '.' symbols which forces all acccounts to be 12
          * characters long without '.' until a future account auction process is implemented
          * which prevents name squatting.
          *
          * 2. new accounts must stake a minimal number of tokens (as set in system parameters)
          * therefore, this method will execute an inline buyram from receiver for newacnt in
          * an amount equal to the current new account creation fee.
          */
         [[dcd::action]]
         void newaccount( const name&       creator,
                          const name&       name,
                          ignore<authority> owner,
                          ignore<authority> active);

         /**
          * Update authorization action updates pemission for an account.
          *
          * @param account - the account for which the permission is updated
          * @param pemission - the permission name which is updated
          * @param parem - the parent of the permission which is updated
          * @param aut - the json describing the permission authorization
          */
         [[dcd::action]]
         void updateauth( ignore<name>      account,
                          ignore<name>      permission,
                          ignore<name>      parent,
                          ignore<authority> auth ) {}

         /**
          * Delete authorization action deletes the authorization for an account's permission.
          *
          * @param account - the account for which the permission authorization is deleted,
          * @param permission - the permission name been deleted.
          */
         [[dcd::action]]
         void deleteauth( ignore<name> account,
                          ignore<name> permission ) {}

         /**
          * Link authorization action assigns a specific action from a contract to a permission you have created. Five system
          * actions can not be linked `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, and `canceldelay`.
          * This is useful because when doing authorization checks, the DCD based blockchain starts with the
          * action needed to be authorized (and the contract belonging to), and looks up which permission
          * is needed to pass authorization validation. If a link is set, that permission is used for authoraization
          * validation otherwise then active is the default, with the exception of `dcd.any`.
          * `dcd.any` is an implicit permission which exists on every account; you can link actions to `dcd.any`
          * and that will make it so linked actions are accessible to any permissions defined for the account.
          *
          * @param account - the permission's owner to be linked and the payer of the RAM needed to store this link,
          * @param code - the owner of the action to be linked,
          * @param type - the action to be linked,
          * @param requirement - the permission to be linked.
          */
         [[dcd::action]]
         void linkauth( ignore<name> account,
                        ignore<name> code,
                        ignore<name> type,
                        ignore<name> requirement  ) {}

         /**
          * Unlink authorization action it's doing the reverse of linkauth action, by unlinking the given action.
          *
          * @param account - the owner of the permission to be unlinked and the receiver of the freed RAM,
          * @param code - the owner of the action to be unlinked,
          * @param type - the action to be unlinked.
          */
         [[dcd::action]]
         void unlinkauth( ignore<name> account,
                          ignore<name> code,
                          ignore<name> type ) {}

         /**
          * Cancel delay action cancels a deferred transaction.
          *
          * @param canceling_auth - the permission that authorizes this action,
          * @param trx_id - the deferred transaction id to be cancelled.
          */
         [[dcd::action]]
         void canceldelay( ignore<permission_level> canceling_auth, ignore<checksum256> trx_id ) {}

         /**
          * On error action, notification of this action is delivered to the sender of a deferred transaction
          * when an objective error occurs while executing the deferred transaction.
          * This action is not meant to be called directly.
          *
          * @param sender_id - the id for the deferred transaction chosen by the sender,
          * @param sent_trx - the deferred transaction that failed.
          */
         [[dcd::action]]
         void onerror( ignore<uint128_t> sender_id, ignore<std::vector<char>> sent_trx );

         /**
          * Set abi action sets the contract abi for an account.
          *
          * @param account - the account for which to set the contract abi.
          * @param abi - the abi content to be set, in the form of a blob binary.
          */
         [[dcd::action]]
         void setabi( const name& account, const std::vector<char>& abi );

         /**
          * Set code action sets the contract code for an account.
          *
          * @param account - the account for which to set the contract code.
          * @param vmtype - reserved, set it to zero.
          * @param vmversion - reserved, set it to zero.
          * @param code - the code content to be set, in the form of a blob binary..
          */
         [[dcd::action]]
         void setcode( const name& account, uint8_t vmtype, uint8_t vmversion, const std::vector<char>& code ) {}

         [[dcd::action]]
         void setfee( const ignore<name> account, 
                      const ignore<name> action, 
                      const ignore<asset> fee ) {}

         [[dcd::action]]
         void setfeeforce( const ignore<double> new_rate) {}



         using newaccount_action = dcd::action_wrapper<"newaccount"_n, &native::newaccount>;
         using updateauth_action = dcd::action_wrapper<"updateauth"_n, &native::updateauth>;
         using deleteauth_action = dcd::action_wrapper<"deleteauth"_n, &native::deleteauth>;
         using linkauth_action = dcd::action_wrapper<"linkauth"_n, &native::linkauth>;
         using unlinkauth_action = dcd::action_wrapper<"unlinkauth"_n, &native::unlinkauth>;
         using canceldelay_action = dcd::action_wrapper<"canceldelay"_n, &native::canceldelay>;
         using setcode_action = dcd::action_wrapper<"setcode"_n, &native::setcode>;
         using setabi_action = dcd::action_wrapper<"setabi"_n, &native::setabi>;
         using setfee_action = dcd::action_wrapper<"setfee"_n, &native::setfee>;
         using setfeeforce_action = dcd::action_wrapper<"setfeeforce"_n, &native::setfeeforce>;
   };
}
