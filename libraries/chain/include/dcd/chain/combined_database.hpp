#pragma once
#include <b1/chain_kv/chain_kv.hpp>
#include <chainbase/chainbase.hpp>
#include <dcd/chain/authorization_manager.hpp>
#include <dcd/chain/block_state.hpp>
#include <dcd/chain/chain_snapshot.hpp>
#include <dcd/chain/fork_database.hpp>
#include <dcd/chain/genesis_state.hpp>
//#include <dcd/chain/resource_limits.hpp>
#include <dcd/chain/snapshot.hpp>

#include <dcd/chain/account_object.hpp>
#include <dcd/chain/backing_store.hpp>
#include <dcd/chain/block_summary_object.hpp>
#include <dcd/chain/code_object.hpp>
#include <dcd/chain/contract_table_objects.hpp>
#include <dcd/chain/database_header_object.hpp>
#include <dcd/chain/generated_transaction_object.hpp>
#include <dcd/chain/genesis_intrinsics.hpp>
#include <dcd/chain/global_property_object.hpp>
#include <dcd/chain/kv_chainbase_objects.hpp>
#include <dcd/chain/protocol_state_object.hpp>
#include <dcd/chain/reversible_block_object.hpp>
#include <dcd/chain/transaction_object.hpp>
#include <dcd/chain/whitelisted_intrinsics.hpp>
#include <dcd/chain/controller.hpp>

#include <b1/session/rocks_session.hpp>
#include <b1/session/session.hpp>
#include <b1/session/undo_stack.hpp>

// It's a fatal condition if chainbase and chain_kv get out of sync with each
// other due to exceptions.
#define CATCH_AND_EXIT_DB_FAILURE()                                                                                    \
   catch (...) {                                                                                                       \
      elog("Error while using database");                                                                              \
      std::abort();                                                                                                    \
   }

namespace dcd { namespace chain {
   using rocks_db_type = dcd::session::session<dcd::session::rocksdb_t>;
   using session_type = dcd::session::session<rocks_db_type>;
   using kv_undo_stack_ptr = std::unique_ptr<dcd::session::undo_stack<rocks_db_type>>;

   using controller_index_set =
         index_set<account_index, account_metadata_index, /*account_ram_correction_index,*/ global_property_multi_index,
                   protocol_state_multi_index, dynamic_global_property_multi_index, block_summary_multi_index,
                   transaction_multi_index, generated_transaction_multi_index, table_id_multi_index, code_index,
                   database_header_multi_index, kv_db_config_index, action_fee_object_index, action_fee_vote_object_index>;

   using contract_database_index_set = index_set<key_value_index, index64_index, index128_index, index256_index,
                                                 index_double_index, index_long_double_index>;

   class apply_context;

   namespace backing_store {
      struct db_context;
   }
   using db_context = backing_store::db_context;

   class combined_session {
    public:
      combined_session() = default;

      combined_session(chainbase::database& cb_database, dcd::session::undo_stack<rocks_db_type>* undo_stack);

      combined_session(combined_session&& src) noexcept;

      ~combined_session() { undo(); }

      combined_session& operator=(const combined_session& src) = delete;

      void push();

      void squash();

      void undo();

    private:
      std::unique_ptr<chainbase::database::session> cb_session    = {};
      dcd::session::undo_stack<rocks_db_type>*     kv_undo_stack = nullptr;
   };

   class combined_database {
    public:
      explicit combined_database(chainbase::database& chain_db,
                                 uint32_t snapshot_batch_threashold);

      combined_database(chainbase::database& chain_db,
                        const controller::config& cfg);

      combined_database(const combined_database& copy) = delete;
      combined_database& operator=(const combined_database& copy) = delete;

      // Save the backing_store setting to the chainbase in order to detect
      // when this setting is switched between chainbase and rocksdb.
      // If existing state is not clean, switching is not allowed.
      void check_backing_store_setting(bool clean_startup);

      static combined_session make_no_op_session() { return combined_session(); }

      combined_session make_session() {
         return combined_session(db, kv_undo_stack.get());
      }

      void set_revision(uint64_t revision);

      int64_t revision();

      void undo();

      void commit(int64_t revision);

      void flush();

      static void destroy(const fc::path& p);

      std::unique_ptr<kv_context> create_kv_context(name receiver, kv_resource_manager resource_manager,
                                                    const kv_database_config& limits)const;

      std::unique_ptr<db_context> create_db_context(apply_context& context, name receiver);

      void add_to_snapshot(const dcd::chain::snapshot_writer_ptr& snapshot, const dcd::chain::block_state& head,
                           const dcd::chain::authorization_manager&                    authorization) const;

      void read_from_snapshot(const snapshot_reader_ptr& snapshot, uint32_t blog_start, uint32_t blog_end,
                              dcd::chain::authorization_manager& authorization,
                              dcd::chain::fork_database& fork_db, dcd::chain::block_state_ptr& head,
                              uint32_t& snapshot_head_block, const dcd::chain::chain_id_type& chain_id);

      auto &get_db(void) const { return db; }
      auto &get_kv_undo_stack(void) const { return kv_undo_stack; }
      backing_store_type get_backing_store() const { return backing_store; }

    private:
      void add_contract_tables_to_snapshot(const snapshot_writer_ptr& snapshot) const;
      void read_contract_tables_from_snapshot(const snapshot_reader_ptr& snapshot);

      backing_store_type                                         backing_store;
      chainbase::database&                                       db;
      std::unique_ptr<rocks_db_type>                             kv_database;
      kv_undo_stack_ptr                                          kv_undo_stack;
      const uint64_t                                             kv_snapshot_batch_threashold;
   };

   std::optional<dcd::chain::genesis_state> extract_legacy_genesis_state(snapshot_reader& snapshot, uint32_t version);

   std::vector<char> make_rocksdb_contract_kv_prefix();
   char make_rocksdb_contract_db_prefix();

}} // namespace dcd::chain
