#pragma once

#include <dcd/asset.hpp>
#include <dcd/dcd.hpp>
#include <dcd/name.hpp>

using dcd::action_wrapper;
using dcd::asset;
using dcd::name;

/**
 * The actions `buyresult`, `sellresult`, `rentresult`, and `orderresult` of `rex.results` are all no-ops.
 * They are added as inline convenience actions to `rentnet`, `rentcpu`, `buyrex`, `unstaketorex`, and `sellrex`.
 * An inline convenience action does not have any effect, however,
 * its data includes the result of the parent action and appears in its trace.
 */
class [[dcd::contract("rex.results")]] rex_results : dcd::contract {
   public:

      using dcd::contract::contract;

      /**
       * Buyresult action.
       *
       * @param rex_received - amount of tokens used in buy order
       */
      [[dcd::action]]
      void buyresult( const asset& rex_received );

      /**
       * Sellresult action.
       *
       * @param proceeds - amount of tokens used in sell order
       */
      [[dcd::action]]
      void sellresult( const asset& proceeds );

      /**
       * Orderresult action.
       *
       * @param owner - the owner of the order
       * @param proceeds - amount of tokens used in order
       */
      [[dcd::action]]
      void orderresult( const name& owner, const asset& proceeds );

      /**
       * Rentresult action.
       *
       * @param rented_tokens - amount of rented tokens
       */
      [[dcd::action]]
      void rentresult( const asset& rented_tokens );

      using buyresult_action   = action_wrapper<"buyresult"_n,   &rex_results::buyresult>;
      using sellresult_action  = action_wrapper<"sellresult"_n,  &rex_results::sellresult>;
      using orderresult_action = action_wrapper<"orderresult"_n, &rex_results::orderresult>;
      using rentresult_action  = action_wrapper<"rentresult"_n,  &rex_results::rentresult>;
};
