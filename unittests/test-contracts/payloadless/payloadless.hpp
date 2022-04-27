#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] payloadless : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void doit();
};
