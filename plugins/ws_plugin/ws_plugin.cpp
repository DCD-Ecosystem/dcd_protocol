#include <dcd/ws_plugin/ws_plugin.hpp>
#include <dcd/chain/exceptions.hpp>
#include <dcd/chain/thread_utils.hpp>
#include <dcd/ws_plugin/ec.hpp>

#include <fc/network/ip.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/crypto/openssl.hpp>


#include <boost/asio.hpp>
#include <boost/optional.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/iostreams/write.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/logger/stub.hpp>

// #include <dcd/chain/config.hpp>
// #include <dcd/chain/exceptions.hpp>
// #include <dcd/chain/transaction.hpp>

#include <thread>
#include <memory>
#include <regex>
#include <shared_mutex>
#include <chrono>

#include <dcd/chain_plugin/chain_plugin.hpp>

namespace bio = boost::iostreams;
using namespace boost::iostreams;

// using namespace std;
// using namespace dcd;
// using namespace dcd::chain;

const fc::string logger_name("ws_plugin");
// fc::logger logger;

namespace dcd {
   #define N(X) dcd::chain::string_to_name(#X)

   struct token_transfer 
   {
      name from;
      name to;
      asset quantity;
      string memo;

      static account_name get_account() 
      {
         return N(dcd.token);
      }

      static action_name get_name() 
      {
         return N(transfer);
      }

   };

   namespace detail {
      /**
       * virtualized wrapper for the various underlying connection functions needed in req/resp processng
       */
      struct abstract_conn {
         virtual ~abstract_conn() = default;
         virtual bool verify_max_bytes_in_flight() = 0;
         virtual bool verify_max_requests_in_flight() = 0;
         virtual void handle_exception() = 0;

         virtual void send_response(std::optional<std::string> body, int code) = 0;
      };

      using abstract_conn_ptr = std::shared_ptr<abstract_conn>;

      template<typename T>
      using connection_ptr = typename websocketpp::server<T>::connection_ptr;

      /**
       * internal url handler that contains more parameters than the handlers provided by external systems
       */
      using internal_url_handler = std::function<void(abstract_conn_ptr, string, string, url_response_callback)>;

      /**
       * Helper method to calculate the "in flight" size of a string
       * @param s - the string
       * @return in flight size of s
       */
      static size_t in_flight_sizeof( const string& s ) {
         return s.size();
      }

      /**
       * Helper method to calculate the "in flight" size of a fc::variant
       * This is an estimate based on fc::raw::pack if that process can be successfully executed
       *
       * @param v - the fc::variant
       * @return in flight size of v
       */
      static size_t in_flight_sizeof( const fc::variant& v ) {
         try {
            return fc::raw::pack_size( v );
         } catch(...) {}
         return 0;
      }

      /**
       * Helper method to calculate the "in flight" size of a std::optional<T>
       * When the optional doesn't contain value, it will return the size of 0
       *
       * @param o - the std::optional<T> where T is typename
       * @return in flight size of o
       */
      template<typename T>
      static size_t in_flight_sizeof( const std::optional<T>& o ) {
         if( o ) {
            return in_flight_sizeof( *o );
         }
         return 0;
      }
   } // detail



   static appbase::abstract_plugin& _ws_plugin = app().register_plugin<ws_plugin>();

   namespace asio = boost::asio;

   using std::map;
   using std::vector;
   using std::set;
   using std::string;
   using std::regex;
   using boost::asio::ip::tcp;
   using boost::asio::ip::address_v4;
   using boost::asio::ip::address_v6;
   using std::shared_ptr;
   using websocketpp::connection_hdl;

   enum ws_ecdh_curve_t {
      SECP384R1,
      PRIME256V1
   };

   static ws_plugin_defaults current_ws_plugin_defaults;

   void ws_plugin::set_defaults(const ws_plugin_defaults& config) {
      current_ws_plugin_defaults = config;
   }

   static bool verbose_http_errors = true;

  #define private_wif "5KApmWa8RNcrFX2tCyDwS3FgJvNRwJsSdqrJGJ3sXBusL1kWgVy"
  #define contract_name "dcdpcontract"
  using ws_plugin_impl_ptr = std::shared_ptr<class ws_plugin_impl>;
   
   class ws_plugin_impl
   {
      public:  

      // /**
      //     * Construct a lambda appropriate for url_response_callback that will
      //     * JSON-stringify the provided response
      //     *
      //     * @param con - pointer for the connection this response should be sent to
      //     * @return lambda suitable for url_response_callback
      //     */
      //    template<typename T>
      //    auto make_http_response_handler( const detail::abstract_conn_ptr& abstract_conn_ptr) {
      //       return [my=shared_from_this(), abstract_conn_ptr]( int code, std::optional<fc::variant> response ) {
      //          auto tracked_response = make_in_flight(std::move(response), my);
      //          if (!abstract_conn_ptr->verify_max_bytes_in_flight()) {
      //             return;
      //          }

      //          // post  back to an HTTP thread to to allow the response handler to be called from any thread
      //          boost::asio::post( my->thread_pool->get_executor(),
      //                             [my, abstract_conn_ptr, code, tracked_response=std::move(tracked_response)]() {
      //             try {
      //                if( tracked_response->obj().has_value() ) {
      //                   std::string json = fc::json::to_string( *tracked_response->obj(), fc::time_point::now() + my->max_response_time );
      //                   auto tracked_json = make_in_flight( std::move( json ), my );
      //                   abstract_conn_ptr->send_response( std::move( tracked_json->obj() ), code );
      //                } else {
      //                   abstract_conn_ptr->send_response( {}, code );
      //                }
      //             } catch( ... ) {
      //                abstract_conn_ptr->handle_exception();
      //             }
      //          });
      //       };
      //    }

      std::atomic<size_t>                            bytes_in_flight{0};

      /**
          * Helper type that wraps an object of type T and records its "in flight" size to
          * http_plugin_impl::bytes_in_flight using RAII semantics
          *
          * @tparam T - the contained Type
          */
         template<typename T>
         struct in_flight {
            in_flight(T&& object, ws_plugin_impl_ptr impl)
            :_object(std::move(object))
            ,_impl(std::move(impl))
            {
               _count = dcd::detail::in_flight_sizeof(_object);
               _impl->bytes_in_flight += _count;
            }

            ~in_flight() {
               if (_count) {
                  _impl->bytes_in_flight -= _count;
               }
            }

            // No copy constructor, but allow move
            in_flight(const in_flight&) = delete;
            in_flight(in_flight&& from)
            :_object(std::move(from._object))
            ,_count(from._count)
            ,_impl(std::move(from._impl))
            {
               from._count = 0;
            }

            // No copy assignment, but allow move
            in_flight& operator=(const in_flight&) = delete;
            in_flight& operator=(in_flight&& from) {
               _object = std::move(from._object);
               _count = from._count;
               _impl = std::move(from._impl);
               from._count = 0;
            }

            /**
             * const accessor
             * @return const reference to the contained object
             */
            const T& obj() const {
               return _object;
            }

            /**
             * mutable accessor (can be moved from)
             * @return mutable reference to the contained object
             */
            T& obj() {
               return _object;
            }

            T _object;
            size_t _count;
            ws_plugin_impl_ptr _impl;
         };

         /**
          * convenient wrapper to make an in_flight<T>
          */
         template<typename T>
         static auto make_in_flight(T&& object, ws_plugin_impl_ptr impl) {
            return std::make_shared<in_flight<T>>(std::forward<T>(object), std::move(impl));
         }

         static dcd::detail::internal_url_handler make_url_handler(int priority, url_handler next, ws_plugin_impl_ptr my ) 
         {
            auto next_ptr = std::make_shared<url_handler>(std::move(next));
            return [my=std::move(my), priority, next_ptr=std::move(next_ptr)]
                  ( dcd::detail::abstract_conn_ptr conn, string r, string b, url_response_callback then ) 
            { /// !!!! не заходит!!
            
               auto tracked_b = make_in_flight<string>(std::move(b), my);
               url_response_callback wrapped_then = [tracked_b, then=std::move(then)](int code, std::optional<fc::variant> resp) {
                  then(code, std::move(resp));
               };            
            

               // post to the app thread taking shared ownership of next (via std::shared_ptr),
               // sole ownership of the tracked body and the passed in parameters
               app().post( priority, [next_ptr, r=std::move(r), tracked_b, wrapped_then=std::move(wrapped_then)]() mutable {
                  try {
                     // call the `next` url_handler and wrap the response handler
                     (*next_ptr)( std::move( r ), std::move(tracked_b->obj()), std::move(wrapped_then)) ;
                  } catch( ... ) {
                  // conn->handle_exception();
                  }
               } );

            };// return
         }// make_url_handler

         /**
          * Make an internal_url_handler that will run the url_handler directly
          *
          * @pre b.size() has been added to bytes_in_flight by caller
          * @param next - the next handler for responses
          * @return the constructed internal_url_handler
          */
         static dcd::detail::internal_url_handler make_http_thread_url_handler(url_handler next) 
         {
            return [next=std::move(next)]( const dcd::detail::abstract_conn_ptr& conn, string r, string b, url_response_callback then ) {
               try {
                  next(std::move(r), std::move(b), std::move(then));
               } catch( ... ) {
                  conn->handle_exception();
               }
             };
         }


      // Echoes back all received WebSocket messages
class session : public std::enable_shared_from_this<session>
{
    tcp::socket socket_;
    websocket::stream<ssl::stream<tcp::socket&>> ws_;
   //  boost::asio::strand<
   //      boost::asio::io_context::executor_type> strand_;
    boost::beast::multi_buffer buffer_;
                dcd::ws_plugin_impl* plugin_;
	         std::string hash_;
      

         public:

    session(tcp::socket socket, ssl::context& ctx, dcd::ws_plugin_impl* plugin)
        : socket_(std::move(socket))
        , ws_(socket_, ctx)
                , plugin_(plugin)
      //   , strand_(ws_.get_executor())

    {

    }



            ~session(){
               std::cout << " session d-tor called!" << std::endl;
            }

  // Start the asynchronous operation
    void
    run()
    {
       // Set suggested timeout settings for the websocket
        ws_.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-async");
            }));

         

        // Perform the SSL handshake
        ws_.next_layer().async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(
                &session::on_handshake,
                shared_from_this()));

      //   ws_.next_layer().async_handshake(
      //       ssl::stream_base::server,
      //       boost::asio::bind_executor(
      //           strand_,
      //           std::bind(
      //               &session::on_handshake,
      //               shared_from_this(),
      //               std::placeholders::_1)));
    }

    void
    on_handshake(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");

        // Accept the websocket handshake
        ws_.async_accept(
            beast::bind_front_handler(
                &session::on_accept,
                shared_from_this()));

      //   ws_.async_accept(
      //       boost::asio::bind_executor(
      //           strand_,
      //           std::bind(
      //               &session::on_accept,
      //               shared_from_this(),
      //               std::placeholders::_1)));
    }

    void
    on_accept(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "accept");

        // Read a message
        do_read();
    }

    void
    do_read()
    {
        // Read a message into our buffer
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &session::on_read,
                shared_from_this()));
      //   ws_.async_read(
      //       buffer_,
      //       boost::asio::bind_executor(
      //           strand_,
      //           std::bind(
      //               &session::on_read,
      //               shared_from_this(),
      //               std::placeholders::_1,
      //               std::placeholders::_2)));
    }
            
            std::string encode64(const std::string& s) 
            {
               std::string base64_padding[] = {"", "==","="};
               namespace bai = boost::archive::iterators;

               std::stringstream os;
               typedef bai::base64_from_binary
               <bai::transform_width<const char *, 6, 8> > base64_enc;

               std::copy(base64_enc(s.c_str()), base64_enc(s.c_str() + s.size()),
                           std::ostream_iterator<char>(os));

               os << base64_padding[s.size() % 3];
               return os.str();
            }

            std::string decode64(const std::string& s) 
            {
               std::string base64_padding[] = {"", "==","="};
               namespace bai = boost::archive::iterators;

               std::stringstream os;

               typedef bai::transform_width<bai::binary_from_base64<const char *>, 8, 6> base64_dec;

               unsigned int size = s.size();
               if (size && s[size - 1] == '=') {
                  --size;
                  if (size && s[size - 1] == '=') --size;
               }
               if (size == 0) return std::string();

               std::copy(base64_dec(s.data()), base64_dec(s.data() + size),
                           std::ostream_iterator<char>(os));

               return os.str();
            }

            std::string string_compress_encode(const std::string &data)
            {
               std::stringstream compressed;
               std::stringstream original;
               original << data;
               boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
               out.push(boost::iostreams::zlib_compressor());
               out.push(original);
               boost::iostreams::copy(out, compressed);
               return encode64( compressed.str( ) );
            }

            std::string string_decompress_decode(const std::string &data)
            {
               std::stringstream compressed_encoded;
               std::stringstream decompressed;
               compressed_encoded << data;
               std::stringstream compressed(decode64(compressed_encoded.str()));
               boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
               in.push(boost::iostreams::zlib_decompressor());
               in.push(compressed);
               boost::iostreams::copy(in, decompressed);
               return decompressed.str();
            }

            void decompress(const std::vector<char> &compressed,
                                  std::vector<char> &decompressed)
            {
               boost::iostreams::filtering_ostream os;

               os.push(boost::iostreams::zlib_decompressor());
               os.push(boost::iostreams::back_inserter(decompressed));

               boost::iostreams::write(os, &compressed[0], compressed.size());
            }
/*
            std::string get_signature( const std::vector<uint8_t>& chain_id, const std::vector<uint8_t>& packed_trx, const std::vector<uint8_t>& private_key ) 
            {
               std::vector<uint8_t> data;
               std::vector<uint8_t> zeroes(32, 0);
               data.insert(data.end(), chain_id.begin(), chain_id.end());
               data.insert(data.end(), packed_trx.begin(), packed_trx.end());
               data.insert(data.end(), zeroes.begin(), zeroes.end());

               std::vector<uint8_t> hash = sha256_impl(data).get_hash();     

               secp256k1_impl secp256;
               std::vector<uint8_t> sign = secp256.sign_data_compact(private_key,hash);
               std::vector<uint8_t> check = sign;

               check.push_back(75);
               check.push_back(49); 

               std::vector<uint8_t> checksum = ::ripemd160(check);

               for( int i = 0; i < 4; i++ )
                  sign.push_back(checksum[i]);

               std::stringstream ss;
               ss << "SIG_K1_";
               ss << fc::to_base58(sign);
               return ss.str();
            }
*/
            std::vector<uint8_t> HexToBytes(std::string hex) 
            {
               std::vector<uint8_t> bytes;
               for (unsigned int i = 0; i < hex.length(); i += 2) 
               {
                  std::string byteString = hex.substr(i, 2);
                  uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
                  bytes.push_back(byte);
               }
               return bytes;
            }

            std::vector<char> from_wif( const string& wif_key )
            {
               auto wif_bytes = chain::from_base58(wif_key);
               FC_ASSERT(wif_bytes.size() >= 5);
               auto key_bytes = vector<char>(wif_bytes.begin() + 1, wif_bytes.end() - 4);
               return key_bytes;
            }

            bool trx_check( const transaction& in_trx, std::string& report )
            {               
               for( auto it = in_trx.actions.begin(); it != in_trx.actions.end(); it++ )
               {
                  report.append( "\n contract = ");
                  report.append( (*it).account.to_string() );

                  report.append( "\n action = ");
                  report.append( (*it).name.to_string() );

                  if( (*it).account.to_string() == std::string( contract_name ) )
                  {
                     if( (*it).name.to_string() != std::string( "withdraw" ) &&
                           (*it).name.to_string() != std::string( "stackout" ) &&
                           (*it).name.to_string() != std::string( "connecttable" ) &&
                           (*it).name.to_string() != std::string( "connectnew" ) &&
                           (*it).name.to_string() != std::string( "outfromtable" ) &&
                           (*it).name.to_string() != std::string( "shuffleddeck" ) &&
                           (*it).name.to_string() != std::string( "crypteddeck" ) &&
                           (*it).name.to_string() != std::string( "act" ) &&
                           (*it).name.to_string() != std::string( "actfold" ) &&
                           (*it).name.to_string() != std::string( "setcardskeys" ) &&
                           (*it).name.to_string() != std::string( "resettable" ) &&
                           (*it).name.to_string() != std::string( "sendendgame" ) &&
                           (*it).name.to_string() != std::string( "sendnewgame" ) &&
                           (*it).name.to_string() != std::string( "sendmsg" ) )
                        return false;
                  }else 
                  {
                     if( (*it).account.to_string() == std::string( "dcd.bank" ) )
                     {
                        if( (*it).name.to_string() != std::string( "transfer" ) && (*it).name.to_string() != std::string( "withdraw" ) )
                           return false;
                  
                        if( (*it).name.to_string() == std::string( "transfer" ) )
                        {
                           token_transfer tt = (*it).data_as<token_transfer>();

                           report.append( "\n from = " );  
                           report.append(tt.from.to_string());         

                           report.append( "\n to = " );  
                           report.append(tt.to.to_string());  

                           if( tt.to.to_string() != std::string( contract_name ) )                  
                              return false;

                           if( tt.from.to_string() == std::string( contract_name ) )
                              return false;
                        }

                        if( (*it).name.to_string() == std::string( "withdraw" ) )                     
                        {
                           
                        }
                     }else
                        return false; 
                  }
               }
               return true;
            }
/*
            std::string make_first_sign( const std::string& src )
            {
               size_t begin = src.find("\"packed_trx\":\"");
               begin += 14;

               size_t end = src.find("\",\"", begin + 1);

               std::string packed_trx = src.substr(begin, end - begin);               
               std::vector<uint8_t> trx_bytes = HexToBytes(packed_trx);

               static std::vector<uint8_t> chain_id_bytes;            
               if( chain_id_bytes.size() == 0)
               {
                  chain_plugin& cp = app().get_plugin<chain_plugin>();   
                  std::string chainid = cp.get_chain_id();
      
                  std::transform(chainid.begin(), chainid.end(), chainid.begin(),
                  [](unsigned char c){ return std::tolower(c); });
                  chain_id_bytes = HexToBytes(chainid);
               }

               static std::vector<char> tmp_buf;               
               if(tmp_buf.size() == 0)
                  tmp_buf = from_wif(private_wif);  

               std::vector<uint8_t> private_key_bytes(tmp_buf.begin(),tmp_buf.end());

               std::vector<char> tmp;               
               decompress(std::vector<char>(trx_bytes.begin(), trx_bytes.end()),tmp);

               auto trx = fc::raw::unpack<transaction>(tmp.data(), tmp.size());

               std::string report;
               if( trx_check(trx, report) == false )
               {
                  std::cout << " WARNING!!!! " << report << std::endl;
                  throw std::runtime_error(" wrong action ");
                  return std::string();
               }
  
               std::vector<unsigned char> tmp1;
               std::copy(tmp.begin(), tmp.end(), std::back_inserter(tmp1));

               std::string signature = get_signature(chain_id_bytes,tmp1,private_key_bytes);

               std::string result = src;
               size_t pos = result.find("{\"signatures\":[");

               if (pos != std::string::npos)
               {
                  result.insert(pos + 15, "\"");
                  result.insert(pos + 16, signature);
                  result.insert(pos + 16 + signature.size(), "\"");
                  result.insert(pos + 17 + signature.size(), ",");
               }         
               return result;
            }
*/
            void post_answer( const std::string& answer )
	         {
                  beast::flat_buffer tmp;
                  hash_.append("\n");
                  hash_.append(answer);

 //                 std::cout << " +++ POSTING ANSER - " << peer_endpoint.address() << ":" << peer_endpoint.port() << std::endl << hash_ << std::endl;

                  boost::beast::ostream(tmp) << string_compress_encode( hash_ );

                  // Echo the message
                  ws_.text(ws_.got_text());

         ws_.async_write(
            tmp.data(),
            beast::bind_front_handler(
                &session::on_write,
                shared_from_this()));

         // ws_.async_write(
         //    tmp.data(),
         //    boost::asio::bind_executor(
         //        strand_,
         //        std::bind(
         //            &session::on_write,
         //            shared_from_this(),
         //            std::placeholders::_1,
         //            std::placeholders::_2)));
	         }

            void on_read( beast::error_code ec, std::size_t bytes_transferred){
               try{
                  boost::ignore_unused(bytes_transferred);

                  // This indicates that the session was closed
                  if(ec == websocket::error::closed)
                        return;

                  if(ec)
                  {
                     std::string err = "on_read failed! - ";
                     err.append(ec.message());
                     throw std::runtime_error(err);
                  }

                  std::stringstream ss;
                  ss << string_decompress_decode( boost::beast::buffers_to_string(buffer_.data())); 

   //               std::cout << " +++ GETTING REQUEST - " << peer_endpoint.address() << ":" << peer_endpoint.port() << std::endl << ss.str() << std::endl;
                  
                  std::string h = "";
                  std::string r = "";
                  std::string t = "";
                  std::string b = "";
                  std::string res = "";

                  std::getline(ss, h);
		            hash_ = h;
                  std::getline(ss, r);

                  while (std::getline(ss, t))
                     b.append(t);

                 // if( r.find("/v1/chain/push_transaction") != std::string::npos)                 
                 //    b = make_first_sign( b );

    //              std::time_t t1 = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    //              std::cout << " TRY TO PROCESSING REQUEST FORM - " << peer_endpoint.address() << ":" << peer_endpoint.port() << " " << std::ctime(&t1) << std::endl;
                  plugin_->process_request(r, b, shared_from_this());

    //              std::time_t t2 = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    //              std::cout << " TRY TO PROCESSING REQUEST FORM - COMPLETE" << peer_endpoint.address() << ":" << peer_endpoint.port() << " " << std::ctime(&t2) << std::endl;
               }
               catch ( const fc::exception& e ){
                  elog( "wss: ${e}", ("e",e.to_detail_string()));
                     post_answer("500");
               } catch ( const std::exception& e ){
                  elog( "wss: ${e}", ("e",e.what()));
                     post_answer("500");
               } catch (...) {
                  elog("error thrown from wss io service");
                     post_answer("500");
               }
            };

            void on_write(beast::error_code ec, std::size_t bytes_transferred){
               try{
                     boost::ignore_unused(bytes_transferred);

                     if(ec)
                     {
                        std::string err = "on_write failed! - ";
                        err.append(ec.message());
                        throw std::runtime_error(err);                       
                     }

                     // Clear the buffer
                     buffer_.consume(buffer_.size());

                     // Do another read
                     do_read();
                  }
                  catch ( const fc::exception& e ){
                     elog( "wss: ${e}", ("e",e.to_detail_string()));
                  } catch ( const std::exception& e ){
                     elog( "wss: ${e}", ("e",e.what()));
                  } catch (...) {
                     elog("error thrown from wss io service");
                  }
               }          
         };
   using io_work_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
         // Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    ssl::context& ctx_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    dcd::ws_plugin_impl* plugin_;

public:
    listener(
        boost::asio::io_context& ioc,
        ssl::context& ctx,
        tcp::endpoint endpoint,
        dcd::ws_plugin_impl* plugin)
        : ctx_(ctx)
        , acceptor_(ioc)
        , socket_(ioc)
        , plugin_(plugin)
    {
        boost::system::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            fail(ec, "open");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(
            boost::asio::socket_base::max_listen_connections, ec);
        if(ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void
    run()
    {
        if(! acceptor_.is_open())
            return;
        do_accept();
    }

    void
    do_accept()
    {
        acceptor_.async_accept(
            socket_,
            std::bind(
                &listener::on_accept,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_accept(boost::system::error_code ec)
    {
        if(ec)
        {
            fail(ec, "accept");
        }
        else
        {
            // Create the session and run it
            std::make_shared<session>(std::move(socket_), ctx_, plugin_)->run();
        }

        // Accept another connection
        do_accept();
    }
};

         // map<string,url_handler>                     url_handlers;
         map<string,dcd::detail::internal_url_handler>  url_handlers;

         //std::atomic<size_t>                         bytes_in_flight{0};

         int const threads = 8;

         // The io_context is required for all I/O
         boost::asio::io_context ioc_ws{threads};
     
         // The SSL context is required, and holds certificates
         ssl::context ctx{ssl::context::sslv23};

         std::vector<std::shared_ptr<listener>> _v_listeners;
         std::vector<std::thread> _v_threads;

       //   optional<boost::asio::thread_pool>     thread_pool;

         uint16_t                                    thread_pool_size = 4;
         std::optional<boost::asio::thread_pool>          thread_pool;
         std::shared_ptr<boost::asio::io_context>    server_ioc;
         std::optional<io_work_t>                         server_ioc_work;

         std::vector<uint8_t> private_key_bytes;
         std::vector<uint8_t> chain_id_bytes;

         unsigned short wss_port = 1111;

         ws_plugin_impl(){
            thread_pool.emplace( thread_pool_size );
            server_ioc = std::make_shared<boost::asio::io_context>();
            server_ioc_work.emplace( boost::asio::make_work_guard(*server_ioc) );
               for( uint16_t i = 0; i < thread_pool_size; ++i ) {
                  boost::asio::post( *thread_pool, [ioc = server_ioc]() { ioc->run(); } );
               }
         }

         ~ws_plugin_impl(){
         }

         void setup(){
            ilog( "ws_plugin setup called!" );

            auto const address = boost::asio::ip::make_address("0.0.0.0");
            auto const port = wss_port;
            // This holds the self-signed certificate used by the server
            load_server_certificate(ctx);

            _v_listeners.emplace_back( std::make_shared<listener>(ioc_ws, ctx, tcp::endpoint{address, port}, this));
            _v_listeners[0]->run();

            _v_threads.reserve(threads);
            for(auto i = 0; i < threads; i++){
               _v_threads.emplace_back(
               [&]{
                  ioc_ws.run();
               });
            }
         }

         void stop()
         {
            ioc_ws.stop();

            for( int i = 0; i < _v_listeners.size(); i++)
               _v_listeners[i].reset();

            _v_listeners.clear();

            for( int i = 0; i < _v_threads.size(); i++)
               _v_threads[i].join();

            _v_threads.clear();
         }   

         void process_request( const std::string& resource, const std::string& body, std::shared_ptr<dcd::ws_plugin_impl::session> _session )
         {
            
	      try{
            /*
               auto handler_itr = url_handlers.find( resource );
               if( handler_itr != url_handlers.end()) 
               {
                  app().post( appbase::priority::high,
                              [&ioc = thread_pool->get_executor(), handler_itr,
                               resource{std::move( resource )}, body{std::move( body )}, _session]() {
                     try {
                        handler_itr->second( resource, body,
                              [&ioc, _session]( int code, fc::variant response_body ) 
                              {
                                 boost::asio::post( ioc, [response_body{std::move( response_body )}, _session, code]() mutable {
                                 std::stringstream ss;
                                 ss << code;
                                 ss << std::endl;
                                 ss << fc::json::to_string( response_body );

                                 _session->post_answer( ss.str());
                                 response_body.clear();
                              });

                           });          
                     } catch( ... ) 
                     {
                        std::cout << " wss plugin kakaya to huinya 1!" << std::endl;
			               _session->post_answer( "kakaya to huinya.... " );
			               std::cout << "try to process exception ok! " << std::endl;
                     }
		            });
               } 
               */
                            // std::string body = con->get_request_body();
               //std::string resource = con->get_uri()->get_resource();
               auto handler_itr = url_handlers.find( resource );
               if( handler_itr != url_handlers.end()) {
                //  con->defer_http_response();
                  bytes_in_flight += body.size();
                  //app().post( appbase::priority::low,
                  //            [ioc = this->server_ioc, &bytes_in_flight = this->bytes_in_flight, handler_itr,
                  //             resource{std::move( resource )}, body{std::move( body )}, _session]() {
                     try {
                        bytes_in_flight -= body.size();
                        handler_itr->second(nullptr, resource, body,
                              [ioc = this->server_ioc, &bytes_in_flight = this->bytes_in_flight, _session]( int code, std::optional<fc::variant> response_body ) {
                           bytes_in_flight += response_body->size();
                           boost::asio::post( *ioc, [ioc, response_body{std::move( response_body )}, &bytes_in_flight, _session, code]() {
                                std::stringstream ss;
                                 ss << code;
                                 ss << std::endl;
                                 ss << fc::json::to_string( response_body , fc::time_point::maximum());

                                 _session->post_answer( ss.str());
                                 //response_body.clear();
                           } );
                        });
                     } catch( ... ) {
                      //  handle_exception<T>( con );
                       // con->send_http_response();
                     }
                  //} );
               }
               else 
               {
                  dlog( "404 - not found: ${ep}", ("ep", resource));
               }
            }catch( ... )
            {
                           std::cout << " !!!!!!!!!!!!!!!!!!!!!!!!!! wss plugin kakaya to huinya 2!" << std::endl;
               _session->post_answer("kakaya to huinya 2" );
            }	   
         }

         static void handle_exception( std::shared_ptr<dcd::ws_plugin_impl::session> session, const int& error_code ) {
	         ilog( "handle excaption called!!!!" );
            string err = "Internal Service error, wss: ";
            std::stringstream ss;
	         ss << error_code; 
            ss << std::endl;
            try {
               try {
                  throw;
               } catch (const fc::exception& e) {
                  err += e.to_detail_string();
                  elog( "${e}", ("e", err));
                  error_results_ws results{websocketpp::http::status_code::internal_server_error,
                                       "Internal Service Error", error_results_ws::error_info_ws(e, verbose_http_errors )};
                  ss << fc::json::to_string( results ,fc::time_point::maximum() );
                  session->post_answer(ss.str());
               } catch (const std::exception& e) {
                  err += e.what();
                  elog( "${e}", ("e", err));
                  error_results_ws results{websocketpp::http::status_code::internal_server_error,
                                       "Internal Service Error", error_results_ws::error_info_ws(fc::exception( FC_LOG_MESSAGE( error, e.what())), verbose_http_errors )};
                  ss <<  fc::json::to_string( results, fc::time_point::maximum() );
                  session->post_answer(ss.str());
               } catch (...) {
                  err += "Unknown Exception";
                  error_results_ws results{websocketpp::http::status_code::internal_server_error,
                                          "Internal Service Error",
                                          error_results_ws::error_info_ws(fc::exception( FC_LOG_MESSAGE( error, "Unknown Exception" )), verbose_http_errors )};
                  ss << fc::json::to_string( results, fc::time_point::maximum() );
                  session->post_answer(ss.str());
               }
            } catch (...) {
               ss << ( R"xxx({"message": "Internal Server Error"})xxx" );
               session->post_answer(ss.str());
               std::cerr << "Exception attempting to handle exception: " << err << std::endl;
            }
         }
      };

   ws_plugin::ws_plugin():my(new ws_plugin_impl()){
      ilog( "ws_plugin ctor called!" );
   }

   ws_plugin::~ws_plugin(){
      ilog( "ws_plugin dtor called!" );      
   }

   void ws_plugin::set_program_options(options_description&, options_description& cfg) {
      ilog( "ws_plugin set_program_options called!" );

      cfg.add_options()
         ( "wss-port", bpo::value<short>()->default_value( 1111 ), "The actual wss port.");
   }

   void ws_plugin::plugin_initialize(const variables_map& options) {   

      try{
         my->wss_port = options.at( "wss-port" ).as<short>();
      }catch( std::exception& e){
         ilog( "ws_plugin port nout founded in config, set default to 1111" );
      }

      my->setup();      
   }

   void ws_plugin::plugin_startup() {
       ilog( "ws_plugin plugin_startup called!" );

      //my->thread_pool.emplace( "wss", my->threads * 4 );
/*
      add_api({{
         std::string("/v1/node/get_supported_apis"),
         [&](string, string body, url_response_callback cb) mutable {
            try {
               if (body.empty()) body = "{}";
               auto result = (*this).get_supported_apis();
               cb(200, fc::variant(result));
            } catch (...) {
               handle_exception("node", "get_supported_apis", body, cb);
            }
         }
      }}); 
*/
            add_api({{
         std::string("/v1/node/get_supported_apis"),
         [&](string, string body, url_response_callback cb) mutable {
            try {
               if (body.empty()) body = "{}";
               auto result = (*this).get_supported_apis();
               cb(200, fc::json::to_string(result, fc::time_point::maximum()));
            } catch (...) {
               handle_exception("node", "get_supported_apis", body, cb);
            }
         }
      }});
   }

   void ws_plugin::plugin_shutdown() {   

      my->stop();

            if( my->thread_pool ) {
         my->thread_pool->stop();
      }

       my.reset();

      //app().post( 0, [me = *my](){} ); // keep my pointer alive until queue is drained
    //  my->ioc_ws.stop();
    //  my->thread_pool->stop();     
    //  my->_v_listeners.clear();
    //  my->_v_threads.clear();

    //  my.release(); 

      ilog( "ws_plugin::plugin_shutdown()" );
   }

   void ws_plugin::handle_sighup() {
      // fc::logger::update( logger_name, logger );
   }
   

   void ws_plugin::add_handler(const string& url, const url_handler& handler, int priority) {       
      ilog( "add api url: ${c}", ("c",url) );
      my->url_handlers[url] = my->make_url_handler(priority, handler, my);
   };

   void ws_plugin::add_async_handler(const string& url, const url_handler& handler) {
      // fc_ilog( logger, "add api url: ${c}", ("c", url) );
      ilog( "add api url: ${c}", ("c",url) );
      my->url_handlers[url] = my->make_http_thread_url_handler(handler);
   }

   void ws_plugin::handle_exception( const char *api_name, const char *call_name, const string& body, url_response_callback cb ) {
      try {
         try {
            throw;
         } catch (chain::unknown_block_exception& e) {
            error_results_ws results{400, "Unknown Block", error_results_ws::error_info_ws(e, verbose_http_errors)};
            cb( 400, fc::json::to_string( results, fc::time_point::maximum() ));
         } catch (chain::unsatisfied_authorization& e) {
            error_results_ws results{401, "UnAuthorized", error_results_ws::error_info_ws(e, verbose_http_errors)};
            cb( 401, fc::json::to_string( results, fc::time_point::maximum() ));
         } catch (chain::tx_duplicate& e) {
            error_results_ws results{409, "Conflict", error_results_ws::error_info_ws(e, verbose_http_errors)};
            cb( 409, fc::json::to_string( results, fc::time_point::maximum() ));
         } catch (fc::eof_exception& e) {
            error_results_ws results{422, "Unprocessable Entity", error_results_ws::error_info_ws(e, verbose_http_errors)};
            cb( 422, fc::json::to_string( results, fc::time_point::maximum() ));
            ilog( "Unable to parse arguments to ${api}.${call}", ("api", api_name)( "call", call_name ));
            ilog("Bad arguments: ${args}", ("args", body));
         } catch (fc::exception& e) {
            error_results_ws results{500, "Internal Service Error", error_results_ws::error_info_ws(e, verbose_http_errors)};
            cb( 500, fc::json::to_string( results, fc::time_point::maximum() ));
            if (e.code() != chain::greylist_net_usage_exceeded::code_value && e.code() != chain::greylist_cpu_usage_exceeded::code_value) {
               ilog( "FC Exception encountered while processing ${api}.${call}",
                     ("api", api_name)( "call", call_name ));
               ilog( "Exception Details: ${e}", ("e", e.to_detail_string()));
            }
         } catch (std::exception& e) {
            error_results_ws results{500, "Internal Service Error", error_results_ws::error_info_ws(fc::exception( FC_LOG_MESSAGE( error, e.what())), verbose_http_errors)};
            cb( 500, fc::json::to_string( results, fc::time_point::maximum() ));
            ilog( "STD Exception encountered while processing ${api}.${call}",
                  ("api", api_name)( "call", call_name ));
            ilog( "Exception Details: ${e}", ("e", e.what()));
         } catch (...) {
            error_results_ws results{500, "Internal Service Error",
               error_results_ws::error_info_ws(fc::exception( FC_LOG_MESSAGE( error, "Unknown Exception" )), verbose_http_errors)};
            cb( 500, fc::json::to_string( results, fc::time_point::maximum() ));
            ilog( "Unknown Exception encountered while processing ${api}.${call}",
                  ("api", api_name)( "call", call_name ));
         }
      } catch (...) {
         std::cerr << "Exception attempting to handle exception for " << api_name << "." << call_name << std::endl;
      }
   }


   bool ws_plugin::verbose_errors()const {
      return verbose_http_errors;
   }

   ws_plugin::get_supported_apis_result ws_plugin::get_supported_apis()const {
      get_supported_apis_result result;

      for (const auto& handler : my->url_handlers) {
         if (handler.first != "/v1/node/get_supported_apis")
            result.apis.emplace_back(handler.first);
      }

      return result;
   }

   std::istream& operator>>(std::istream& in, ws_ecdh_curve_t& curve) {
      std::string s;
      in >> s;
      if (s == "secp384r1")
         curve = SECP384R1;
      else if (s == "prime256v1")
         curve = PRIME256V1;
      else
         in.setstate(std::ios_base::failbit);
      return in;
   }

   std::ostream& operator<<(std::ostream& osm, ws_ecdh_curve_t curve) {
      if (curve == SECP384R1) {
         osm << "secp384r1";
      } else if (curve == PRIME256V1) {
         osm << "prime256v1";
      }

      return osm;
   }
}

FC_REFLECT( dcd::token_transfer , (from)(to)(quantity)(memo) )


