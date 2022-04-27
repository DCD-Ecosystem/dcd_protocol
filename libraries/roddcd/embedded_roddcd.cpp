#include <b1/roddcd/embedded_roddcd.h>
#include <b1/roddcd/roddcd.hpp>
#include <fc/scoped_exit.hpp>

struct roddcd_error_s {
   const char* msg = "no error";
   std::string buffer;

   bool set(const char* m) {
      try {
         buffer = m;
         msg    = buffer.c_str();
      } catch (...) { msg = "error storing error message"; }
      return false;
   }
};

struct roddcd_context_s : b1::roddcd::roddcd_context {};

struct roddcd_db_partition_s {
   std::shared_ptr<b1::roddcd::roddcd_db_partition> obj;
};

struct roddcd_db_snapshot_s : b1::roddcd::roddcd_db_snapshot {
   using roddcd_db_snapshot::roddcd_db_snapshot;
};

struct roddcd_filter_s : b1::roddcd::roddcd_filter {
   using roddcd_filter::roddcd_filter;
};

struct roddcd_query_handler_s : b1::roddcd::roddcd_query_handler {
   using roddcd_query_handler::roddcd_query_handler;
};

extern "C" roddcd_error* roddcd_create_error() {
   try {
      return std::make_unique<roddcd_error>().release();
   } catch (...) { return nullptr; }
}

extern "C" void roddcd_destroy_error(roddcd_error* error) { std::unique_ptr<roddcd_error>{ error }; }

extern "C" const char* roddcd_get_error(roddcd_error* error) {
   if (!error)
      return "error is null";
   return error->msg;
}

template <typename T, typename F>
auto handle_exceptions(roddcd_error* error, T errval, F f) noexcept -> decltype(f()) {
   if (!error)
      return errval;
   try {
      return f();
   } catch (std::exception& e) {
      error->set(e.what());
      return errval;
   } catch (...) {
      error->set("unknown exception");
      return errval;
   }
}

extern "C" roddcd_context* roddcd_create() {
   try {
      return std::make_unique<roddcd_context>().release();
   } catch (...) { return nullptr; }
}

extern "C" void roddcd_destroy(roddcd_context* context) { std::unique_ptr<roddcd_context>{ context }; }

extern "C" roddcd_bool roddcd_open_db(roddcd_error* error, roddcd_context* context, const char* path,
                                      roddcd_bool create_if_missing, int num_threads, int max_open_files) {
   return handle_exceptions(error, false, [&] {
      if (!context)
         return error->set("context is null");
      if (!path)
         return error->set("path is null");
      if (context->db)
         return error->set("a database is already open on this context");
      context->db = std::make_shared<b1::chain_kv::database>(
            path, create_if_missing, num_threads ? std::make_optional(num_threads) : std::nullopt,
            max_open_files ? std::make_optional(max_open_files) : std::nullopt);
      return true;
   });
}

extern "C" roddcd_db_partition* roddcd_create_partition(roddcd_error* error, roddcd_context* context,
                                                        const char* prefix, uint32_t prefix_size) {
   return handle_exceptions(error, nullptr, [&]() -> roddcd_db_partition* {
      if (!context)
         return error->set("context is null"), nullptr;
      if (!prefix)
         return error->set("prefix is null"), nullptr;
      if (!context->db)
         return error->set("database wasn't opened"), nullptr;
      auto p = std::make_unique<roddcd_db_partition>();
      p->obj = std::make_shared<b1::roddcd::roddcd_db_partition>(context->db,
                                                                 std::vector<char>{ prefix, prefix + prefix_size });
      return p.release();
   });
}

extern "C" void roddcd_destroy_partition(roddcd_db_partition* partition) {
   std::unique_ptr<roddcd_db_partition>{ partition };
}

extern "C" roddcd_db_snapshot* roddcd_create_snapshot(roddcd_error* error, roddcd_db_partition* partition,
                                                      roddcd_bool persistent) {
   return handle_exceptions(error, nullptr, [&]() -> roddcd_db_snapshot* {
      if (!partition)
         return error->set("partition is null"), nullptr;
      return std::make_unique<roddcd_db_snapshot>(partition->obj, persistent).release();
   });
}

extern "C" void roddcd_destroy_snapshot(roddcd_db_snapshot* snapshot) {
   std::unique_ptr<roddcd_db_snapshot>{ snapshot };
}

extern "C" roddcd_bool roddcd_refresh_snapshot(roddcd_error* error, roddcd_db_snapshot* snapshot) {
   return handle_exceptions(error, false, [&]() {
      if (!snapshot)
         return error->set("snapshot is null");
      snapshot->refresh();
      return true;
   });
}

template <typename F>
void with_result(const char* data, uint64_t size, F f) {
   dcd::input_stream          bin{ data, data + size };
   dcd::ship_protocol::result result;
   from_bin(result, bin);
   auto* result_v0 = std::get_if<dcd::ship_protocol::get_blocks_result_v0>(&result);
   if (result_v0)
      return f(*result_v0);

   auto* result_v1 = std::get_if<dcd::ship_protocol::get_blocks_result_v1>(&result);
   if (result_v1)
      return f(*result_v1);

   throw std::runtime_error("expected a get_blocks_result_v0 or get_blocks_result_v1");
}

extern "C" roddcd_bool roddcd_start_block(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data,
                                          uint64_t size) {
   return handle_exceptions(error, false, [&]() {
      if (!snapshot)
         return error->set("snapshot is null");
      with_result(data, size, [&](auto& result) { snapshot->start_block(result); });
      return true;
   });
}

extern "C" roddcd_bool roddcd_end_block(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data,
                                        uint64_t size, bool force_write) {
   return handle_exceptions(error, false, [&]() {
      if (!snapshot)
         return error->set("snapshot is null");
      with_result(data, size, [&](auto& result) { snapshot->end_block(result, force_write); });
      return true;
   });
}

extern "C" roddcd_bool roddcd_write_block_info(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data,
                                               uint64_t size) {
   return handle_exceptions(error, false, [&]() {
      if (!snapshot)
         return error->set("snapshot is null");
      with_result(data, size, [&](auto& result) { snapshot->write_block_info(result); });
      return true;
   });
}

extern "C" roddcd_bool roddcd_write_deltas(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data,
                                           uint64_t size, roddcd_bool (*shutdown)(void*), void* shutdown_arg) {
   return handle_exceptions(error, false, [&]() {
      if (!snapshot)
         return error->set("snapshot is null");
      with_result(data, size, [&](auto& result) {
         snapshot->write_deltas(result, [=]() -> bool {
            if (shutdown)
               return shutdown(shutdown_arg);
            else
               return false;
         });
      });
      return true;
   });
}

extern "C" roddcd_filter* roddcd_create_filter(roddcd_error* error, uint64_t name, const char* wasm_filename) {
   return handle_exceptions(error, nullptr, [&]() -> roddcd_filter* { //
      return std::make_unique<roddcd_filter>(dcd::name{ name }, wasm_filename).release();
   });
}

extern "C" void roddcd_destroy_filter(roddcd_filter* filter) { std::unique_ptr<roddcd_filter>{ filter }; }

extern "C" roddcd_bool roddcd_run_filter(roddcd_error* error, roddcd_db_snapshot* snapshot, roddcd_filter* filter,
                                         const char* data, uint64_t size,
                                         roddcd_bool (*push_data)(void* arg, const char* data, uint64_t size),
                                         void* push_data_arg) {
   return handle_exceptions(error, false, [&]() {
      if (!snapshot)
         return error->set("snapshot is null");
      if (!filter)
         return error->set("filter is null");
      with_result(data, size, [&](auto& result) {
         filter->process(*snapshot, result, { data, data + size }, [&](const char* data, uint64_t size) {
            if (push_data && !push_data(push_data_arg, data, size))
               throw std::runtime_error("push_data returned false");
         });
      });
      return true;
   });
}

extern "C" roddcd_query_handler* roddcd_create_query_handler(roddcd_error* error, roddcd_db_partition* partition,
                                                             uint32_t max_console_size, uint32_t wasm_cache_size,
                                                             uint64_t max_exec_time_ms, const char* contract_dir) {
   return handle_exceptions(error, nullptr, [&]() -> roddcd_query_handler* {
      if (!partition)
         return error->set("partition is null"), nullptr;
      auto shared_state              = std::make_shared<b1::roddcd::wasm_ql::shared_state>(partition->obj->db);
      shared_state->max_console_size = max_console_size;
      shared_state->wasm_cache_size  = wasm_cache_size;
      shared_state->max_exec_time_ms = max_exec_time_ms;
      shared_state->contract_dir     = contract_dir ? contract_dir : "";
      return std::make_unique<roddcd_query_handler>(partition->obj, shared_state).release();
   });
}

void roddcd_destroy_query_handler(roddcd_query_handler* handler) { std::unique_ptr<roddcd_query_handler>{ handler }; }

roddcd_bool roddcd_query_transaction(roddcd_error* error, roddcd_query_handler* handler, roddcd_db_snapshot* snapshot,
                                     const char* data, uint64_t size, char** result, uint64_t* result_size) {
   return handle_exceptions(error, false, [&]() {
      if (!handler)
         return error->set("handler is null");
      if (!result)
         return error->set("result is null");
      if (!result_size)
         return error->set("result_size is null");
      *result      = nullptr;
      *result_size = 0;

      std::vector<std::vector<char>> memory;
      dcd::input_stream            s{ data, size };
      auto trx = dcd::from_bin<dcd::ship_protocol::packed_transaction>(s);

      auto                                    thread_state = handler->state_cache.get_state();
      dcd::ship_protocol::transaction_trace tt;
      if (snapshot->snap) {
         tt = query_send_transaction(*thread_state, snapshot->partition->contract_kv_prefix, trx,
                                     snapshot->snap->snapshot(), memory, true);
      } else {
         tt = query_send_transaction(*thread_state, snapshot->partition->contract_kv_prefix, trx, nullptr, memory,
                                     true);
      }

      handler->state_cache.store_state(std::move(thread_state));

      dcd::size_stream ss;
      dcd::to_bin(tt, ss);
      *result = (char*)malloc(ss.size);
      if (!result)
         throw std::bad_alloc();
      auto free_on_except = fc::make_scoped_exit([&]{
         free(*result);
         *result = nullptr;
      });
      dcd::fixed_buf_stream fbs(*result, ss.size);
      to_bin(tt, fbs);
      if (fbs.pos != fbs.end) {
         dcd::check(false, dcd::convert_stream_error(dcd::stream_error::underrun));
      }
      *result_size = ss.size;
      free_on_except.cancel();
      return true;
   });
}

void roddcd_free_result(char* result) {
   if (result)
      free(result);
}
