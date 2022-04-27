#include <dcd/chain/webassembly/interface.hpp>
#include <dcd/chain/apply_context.hpp>

namespace dcd { namespace chain { namespace webassembly {
   int32_t interface::get_active_producers( legacy_span<account_name> producers ) const {
      auto active_producers = context.get_active_producers();

      size_t len = active_producers.size();
      auto s = len * sizeof(chain::account_name);
      if( producers.size_bytes() == 0 ) return s;

      auto copy_size = std::min( producers.size(), s );
      std::memcpy( producers.data(), active_producers.data(), copy_size );

      return copy_size;
   }
   int64_t interface::set_proposed_rate( legacy_span<char> new_rate, uint32_t datalen ) {
         datastream<const char*> ds( new_rate.data(), new_rate.size() );
         double rate;
         fc::raw::unpack( ds, rate );
         EOS_ASSERT( rate > 0, wasm_execution_error, "rate must be lager than zero!");
         return context.control.set_proposed_rate( rate );
      }

   
}}} // ns dcd::chain::webassembly
