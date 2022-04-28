#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] asserter : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void procassert( int8_t condition, std::string message );

   [[dcd::action]]
   void provereset();
};
