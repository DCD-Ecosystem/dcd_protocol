#include <dcd.system/native.hpp>

#include <dcd/check.hpp>

namespace dcdsystem {

   void native::onerror( ignore<uint128_t>, ignore<std::vector<char>> ) {
      dcd::check( false, "the onerror action cannot be called directly" );
   }

}
