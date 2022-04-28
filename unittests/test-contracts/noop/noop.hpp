#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] noop : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void anyaction( dcd::name                       from,
                   const dcd::ignore<std::string>& type,
                   const dcd::ignore<std::string>& data );
};
