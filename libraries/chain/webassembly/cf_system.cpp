#include <dcd/chain/webassembly/interface.hpp>
#include <dcd/chain/apply_context.hpp>

namespace dcd { namespace chain { namespace webassembly {
   inline static constexpr size_t max_assert_message = 1024;
   void interface::abort() const {
      EOS_ASSERT( false, abort_called, "abort() called" );
   }

   void interface::dcd_assert( bool condition, null_terminated_ptr msg ) const {
      if( BOOST_UNLIKELY( !condition ) ) {
         const size_t sz = strnlen( msg.data(), max_assert_message );
         std::string message( msg.data(), sz );
         EOS_THROW( dcd_assert_message_exception, "assertion failure with message: ${s}", ("s",message) );
      }
   }

   void interface::dcd_assert_message( bool condition, legacy_span<const char> msg ) const {
      if( BOOST_UNLIKELY( !condition ) ) {
         const size_t sz = msg.size() > max_assert_message ? max_assert_message : msg.size();
         std::string message( msg.data(), sz );
         EOS_THROW( dcd_assert_message_exception, "assertion failure with message: ${s}", ("s",message) );
      }
   }

   void interface::dcd_assert_code( bool condition, uint64_t error_code ) const {
      if( BOOST_UNLIKELY( !condition ) ) {
         if( error_code >= static_cast<uint64_t>(system_error_code::generic_system_error) ) {
            restricted_error_code_exception e( FC_LOG_MESSAGE(
                                                   error,
                                                   "dcd_assert_code called with reserved error code: ${error_code}",
                                                   ("error_code", error_code)
            ) );
            e.error_code = static_cast<uint64_t>(system_error_code::contract_restricted_error_code);
            throw e;
         } else {
            dcd_assert_code_exception e( FC_LOG_MESSAGE(
                                             error,
                                             "assertion failure with error code: ${error_code}",
                                             ("error_code", error_code)
            ) );
            e.error_code = error_code;
            throw e;
         }
      }
   }

   void interface::dcd_exit( int32_t code ) const {
      context.control.get_wasm_interface().exit();
   }
}}} // ns dcd::chain::webassembly
