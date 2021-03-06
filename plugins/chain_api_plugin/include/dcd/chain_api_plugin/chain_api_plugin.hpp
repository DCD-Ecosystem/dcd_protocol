#pragma once
#include <dcd/chain_plugin/chain_plugin.hpp>
#include <dcd/http_plugin/http_plugin.hpp>
#include <dcd/ws_plugin/ws_plugin.hpp>

#include <appbase/application.hpp>
#include <dcd/chain/controller.hpp>

namespace dcd {
   using dcd::chain::controller;
   using std::unique_ptr;
   using namespace appbase;

   class chain_api_plugin : public plugin<chain_api_plugin> {
      public:
        APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin)(ws_plugin))

        chain_api_plugin();
        virtual ~chain_api_plugin();

        virtual void set_program_options(options_description&, options_description&) override;

        void plugin_initialize(const variables_map&);
        void plugin_startup();
        void plugin_shutdown();

      private:
        unique_ptr<class chain_api_plugin_impl> my;
   };

}
