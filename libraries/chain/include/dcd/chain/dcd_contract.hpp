#pragma once

#include <dcd/chain/types.hpp>
#include <dcd/chain/contract_types.hpp>

namespace dcd { namespace chain {

   class apply_context;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{
   void apply_dcd_newaccount(apply_context&);
   void apply_dcd_updateauth(apply_context&);
   void apply_dcd_deleteauth(apply_context&);
   void apply_dcd_linkauth(apply_context&);
   void apply_dcd_unlinkauth(apply_context&);

   /*
   void apply_dcd_postrecovery(apply_context&);
   void apply_dcd_passrecovery(apply_context&);
   void apply_dcd_vetorecovery(apply_context&);
   */

   void apply_dcd_setcode(apply_context&);
   void apply_dcd_setabi(apply_context&);

   void apply_dcd_canceldelay(apply_context&);
   void apply_dcd_setfee(apply_context&);
   void apply_dcd_setrate(apply_context&);
   void apply_dcd_procfeeprop(apply_context&);
   void apply_dcd_rmfeeprop(apply_context&);

   ///@}  end action handlers

} } /// namespace dcd::chain
