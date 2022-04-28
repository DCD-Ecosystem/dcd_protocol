#pragma once

#include <optional> // std::optional
#include <utility> // std::pair
#include <appbase/application.hpp> // appbase::plugin
#include "blockvault.hpp"

namespace dcd {

using namespace appbase;
using namespace dcd;
using namespace dcd::chain;

using block_num = uint32_t;

class blockvault_client_plugin_impl;

/// Documentation for blockvault_client_plugin.
///
/// blockvault_client_plugin is a proposed clustered component in an DCD
/// network architecture which provides a replicated durable storage with strong
/// consistency guarantees for all the input required by a redundant cluster of
/// dcdnode nodes to achieve the guarantees:
///
/// Guarantee against double-production of blocks
/// Guarantee against finality violation
/// Guarantee of liveness (ability to make progress as a blockchain)
class blockvault_client_plugin : public appbase::plugin<blockvault_client_plugin> {
public:
   blockvault_client_plugin();
   virtual ~blockvault_client_plugin();

   APPBASE_PLUGIN_REQUIRES()
   virtual void set_program_options(options_description&, options_description& cfg) final;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();
   dcd::blockvault::block_vault_interface* get();

   void handle_sighup() override;

 private:
   std::unique_ptr<class blockvault_client_plugin_impl> my;
};

}
