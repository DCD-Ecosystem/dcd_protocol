/**
 *  @file
 *  @copyright defined in dcd/LICENSE
 */
#pragma once
#include <appbase/application.hpp>
#include <fc/exception/exception.hpp>

#include <fc/reflect/reflect.hpp>

#include "server_certificate.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <dcd/ws_plugin/ws_plugin.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;               // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

inline void fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

namespace dcd {
   using namespace appbase;

   /**
    * @brief A callback function provided to a URL handler to
    * allow it to specify the HTTP response code and body
    *
    * Arguments: response_code, response_body
    */
  // using url_response_callback = std::function<void(int,fc::variant)>;
      //using url_response_callback = std::function<void(int,string)>;
      using url_response_callback = std::function<void(int,std::optional<fc::variant>)>;

   /**
    * @brief Callback type for a URL handler
    *
    * URL handlers have this type
    *
    * The handler must gaurantee that url_response_callback() is called;
    * otherwise, the connection will hang and result in a memory leak.
    *
    * Arguments: url, request_body, response_callback
    **/
   using url_handler = std::function<void(string,string,url_response_callback)>;


   /**
    * @brief An API, containing URLs and handlers
    *
    * An API is composed of several calls, where each call has a URL and
    * a handler. The URL is the path on the web server that triggers the
    * call, and the handler is the function which implements the API call
    */
   using api_description = std::map<string, url_handler>;

   struct ws_plugin_defaults {
      //If empty, unix socket support will be completely disabled. If not empty,
      // unix socket support is enabled with the given default path (treated relative
      // to the datadir)
      string default_unix_socket_path;
      //If non 0, HTTP will be enabled by default on the given port number. If
      // 0, HTTP will not be enabled by default
      uint16_t default_http_port{0};
   };

   /**
    *  This plugin starts an HTTP server and dispatches queries to
    *  registered handles based upon URL. The handler is passed the
    *  URL that was requested and a callback method that should be
    *  called with the response code and body.
    *
    *  The handler will be called from the appbase application io_service
    *  thread.  The callback can be called from any thread and will
    *  automatically propagate the call to the http thread.
    *
    *  The HTTP service will run in its own thread with its own io_service to
    *  make sure that HTTP request processing does not interfer with other
    *  plugins.
    */
   class ws_plugin : public appbase::plugin<ws_plugin>
   {
      public:
        ws_plugin();
        ~ws_plugin() override;

        //must be called before initialize
        static void set_defaults(const ws_plugin_defaults& config);

        APPBASE_PLUGIN_REQUIRES()
        void set_program_options(options_description&, options_description& cfg) override;

        void plugin_initialize(const variables_map& options);
        void plugin_startup();
        void plugin_shutdown();
        void handle_sighup() override;

        void add_handler(const string& url, const url_handler&, int priority = appbase::priority::medium_low);
        void add_api(const api_description& api, int priority = appbase::priority::medium_low) {
           for (const auto& call : api)
              add_handler(call.first, call.second, priority);
        }

        void add_async_handler(const string& url, const url_handler& handler);
        void add_async_api(const api_description& api) {
           for (const auto& call : api)
              add_handler(call.first, call.second);
        }

        // standard exception handling for api handlers
        static void handle_exception( const char *api_name, const char *call_name, const string& body, url_response_callback cb );
        
        bool verbose_errors()const;

        struct get_supported_apis_result {
           vector<string> apis;
        };

        get_supported_apis_result get_supported_apis()const;

      private:
         std::shared_ptr<class ws_plugin_impl> my;

   };

   /**
    * @brief Structure used to create JSON error responses
    */
   
   struct error_results_ws {
      uint16_t code;
      string message;

      struct error_info_ws {
         int64_t code;
         string name;
         string what;

         struct error_detail_ws {
            string message;
            string file;
            uint64_t line_number;
            string method;
         };

         vector<error_detail_ws> details;

         static const uint8_t details_limit = 10;

         error_info_ws() {};

         error_info_ws(const fc::exception& exc, bool include_full_log) {
            code = exc.code();
            name = exc.name();
            what = exc.what();
            uint8_t limit = include_full_log ? details_limit : 1;
            for( auto itr = exc.get_log().begin(); itr != exc.get_log().end(); ++itr ) {
               // Prevent sending trace that are too big
               if( details.size() >= limit ) break;
               // Append error
               error_detail_ws detail = {
                     include_full_log ? itr->get_message() : itr->get_limited_message(),
                     itr->get_context().get_file(),
                     itr->get_context().get_line_number(),
                     itr->get_context().get_method()
               };
               details.emplace_back( detail );
            }
         }
      };

      error_info_ws error;
   };
}

FC_REFLECT(dcd::error_results_ws::error_info_ws::error_detail_ws, (message)(file)(line_number)(method))
FC_REFLECT(dcd::error_results_ws::error_info_ws, (code)(name)(what)(details))
FC_REFLECT(dcd::error_results_ws, (code)(message)(error))
FC_REFLECT(dcd::ws_plugin::get_supported_apis_result, (apis))


