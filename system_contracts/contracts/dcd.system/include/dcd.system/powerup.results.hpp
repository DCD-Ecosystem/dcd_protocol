#pragma once

#include <dcd/asset.hpp>
#include <dcd/dcd.hpp>
#include <dcd/name.hpp>

using dcd::action_wrapper;
using dcd::asset;
using dcd::name;

/**
 * The action `powerresult` of `power.results` is a no-op.
 * It is added as an inline convenience action to `powerup` reservation.
 * This inline convenience action does not have any effect, however,
 * its data includes the result of the parent action and appears in its trace.
 */
class [[dcd::contract("powup.results")]] powup_results : dcd::contract {
   public:

      using dcd::contract::contract;

      /**
       * powupresult action.
       *
       * @param fee       - powerup fee amount
       * @param powup_net - amount of powup NET tokens
       * @param powup_cpu - amount of powup CPU tokens
       */
      [[dcd::action]]
      void powupresult( const asset& fee, const int64_t powup_net, const int64_t powup_cpu );

      using powupresult_action  = action_wrapper<"powupresult"_n,  &powup_results::powupresult>;
};
