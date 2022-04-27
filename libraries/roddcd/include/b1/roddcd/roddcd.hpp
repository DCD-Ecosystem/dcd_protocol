#pragma once
#include <b1/chain_kv/chain_kv.hpp>
#include <b1/roddcd/filter.hpp>
#include <b1/roddcd/wasm_ql.hpp>
#include <dcd/ship_protocol.hpp>
#include <functional>

namespace b1::roddcd {

static constexpr char undo_prefix_byte        = 0x01;
static constexpr char contract_kv_prefix_byte = 0x02;

struct roddcd_context {
   std::shared_ptr<chain_kv::database> db;
};

struct roddcd_db_partition {
   const std::shared_ptr<chain_kv::database> db;
   const std::vector<char>                   undo_prefix;
   const std::vector<char>                   contract_kv_prefix;

   // todo: move rocksdb::ManagedSnapshot to here to prevent optimization in cloner from
   //       defeating non-persistent snapshots.

   roddcd_db_partition(std::shared_ptr<chain_kv::database> db, const std::vector<char>& prefix)
       : db{ std::move(db) }, //
         undo_prefix{ [&] {
            auto x = prefix;
            x.push_back(undo_prefix_byte);
            return x;
         }() },
         contract_kv_prefix{ [&] {
            auto x = prefix;
            x.push_back(contract_kv_prefix_byte);
            return x;
         }() } {}
};

struct roddcd_db_snapshot {
   std::shared_ptr<roddcd_db_partition>    partition       = {};
   std::shared_ptr<chain_kv::database>     db              = {};
   std::optional<chain_kv::undo_stack>     undo_stack      = {}; // only if persistent
   std::optional<rocksdb::ManagedSnapshot> snap            = {}; // only if !persistent
   std::optional<chain_kv::write_session>  write_session   = {};
   dcd::checksum256                      chain_id        = {};
   uint32_t                                head            = 0;
   dcd::checksum256                      head_id         = {};
   uint32_t                                irreversible    = 0;
   dcd::checksum256                      irreversible_id = {};
   uint32_t                                first           = 0;
   std::optional<uint32_t>                 writing_block   = {};

   roddcd_db_snapshot(std::shared_ptr<roddcd_db_partition> partition, bool persistent);

   void refresh();
   void end_write(bool write_fill);
   void start_block(const dcd::ship_protocol::get_blocks_result_base& result);
   void end_block(const dcd::ship_protocol::get_blocks_result_base& result, bool force_write);
   void check_write(const dcd::ship_protocol::get_blocks_result_base& result);
   void write_block_info(const dcd::ship_protocol::get_blocks_result_v0& result);
   void write_block_info(const dcd::ship_protocol::get_blocks_result_v1& result);
   void write_deltas(const dcd::ship_protocol::get_blocks_result_v0& result, std::function<bool()> shutdown);
   void write_deltas(const dcd::ship_protocol::get_blocks_result_v1& result, std::function<bool()> shutdown);

 private:
   void write_block_info(uint32_t block_num, const dcd::checksum256& id,
                         const dcd::ship_protocol::signed_block_header& block);
   void write_deltas(uint32_t block_num, dcd::opaque<std::vector<dcd::ship_protocol::table_delta>> deltas, std::function<bool()> shutdown);
   void write_fill_status();
};

struct roddcd_filter {
   dcd::name                           name         = {};
   std::unique_ptr<filter::backend_t>    backend      = {};
   std::unique_ptr<filter::filter_state> filter_state = {};

   roddcd_filter(dcd::name name, const std::string& wasm_filename);

   void process(roddcd_db_snapshot& snapshot, const dcd::ship_protocol::get_blocks_result_base& result,
                dcd::input_stream bin, const std::function<void(const char* data, uint64_t size)>& push_data);
};

struct roddcd_query_handler {
   std::shared_ptr<roddcd_db_partition>               partition;
   const std::shared_ptr<const wasm_ql::shared_state> shared_state;
   wasm_ql::thread_state_cache                        state_cache;

   roddcd_query_handler(std::shared_ptr<roddcd_db_partition>         partition,
                        std::shared_ptr<const wasm_ql::shared_state> shared_state);
   roddcd_query_handler(const roddcd_query_handler&) = delete;
};

} // namespace b1::roddcd
