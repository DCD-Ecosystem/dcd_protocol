#include <dcd.boot/dcd.boot.hpp>
#include <dcd/privileged.hpp>

namespace dcdboot {

void boot::onerror( ignore<uint128_t>, ignore<std::vector<char>> ) {
   check( false, "the onerror action cannot be called directly" );
}

void boot::activate( const dcd::checksum256& feature_digest ) {
   require_auth( get_self() );
   dcd::preactivate_feature( feature_digest );
}

void boot::reqactivated( const dcd::checksum256& feature_digest ) {
   check( dcd::is_feature_activated( feature_digest ), "protocol feature is not activated" );
}

}
