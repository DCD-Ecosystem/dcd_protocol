#include <dcd/chain/apply_context.hpp>
#include <dcd/chain/exceptions.hpp>
#include <dcd/chain/kv_chainbase_objects.hpp>
#include <dcd/chain/backing_store/kv_context.hpp>

namespace dcd { namespace chain {

   struct kv_iterator_chainbase : kv_iterator {
      using index_type = std::decay_t<decltype(std::declval<chainbase::database>().get_index<kv_index, by_kv_key>())>;
      using tracker_type = std::decay_t<decltype(std::declval<chainbase::database>().get_mutable_index<kv_index>().track_removed())>;

      chainbase::database&       db;
      const index_type&          idx = db.get_index<kv_index, by_kv_key>();
      tracker_type&              tracker;
      uint32_t&                  itr_count;
      name                       contract;
      std::vector<char>          prefix;
      const kv_object*           current = nullptr;

      kv_iterator_chainbase(chainbase::database& db, tracker_type& tracker, uint32_t& itr_count, name contract, std::vector<char> prefix)
         : db{ db }, tracker{ tracker }, itr_count(itr_count), contract{ contract }, prefix{ std::move(prefix) } {

         ++itr_count;
      }

      ~kv_iterator_chainbase() override { --itr_count; }

      bool is_kv_chainbase_context_iterator() const override { return true; }
      bool is_kv_rocksdb_context_iterator() const override { return false; }

      template <typename It>
      kv_it_stat move_to(const It& it, uint32_t* found_key_size, uint32_t* found_value_size) {
         if (it != idx.end() && it->contract == contract &&
             it->kv_key.size() >= prefix.size() && !memcmp(it->kv_key.data(), prefix.data(), prefix.size())) {
            current = &*it;
            *found_key_size = current->kv_key.size();
            *found_value_size = current->kv_value.size();
            return kv_it_stat::iterator_ok;
         } else {
            current = nullptr;
            *found_key_size = 0;
            *found_value_size = 0;
            return kv_it_stat::iterator_end;
         }
      }

      kv_it_stat kv_it_status() override {
         if (!current)
            return kv_it_stat::iterator_end;
         else if (!tracker.is_removed(*current))
            return kv_it_stat::iterator_ok;
         else
            return kv_it_stat::iterator_erased;
      }

      int32_t kv_it_compare(const kv_iterator& rhs) override {
         DCD_ASSERT(rhs.is_kv_chainbase_context_iterator(), kv_bad_iter, "Incompatible key-value iterators");
         auto& r = static_cast<const kv_iterator_chainbase&>(rhs);
         
         DCD_ASSERT(contract == r.contract, kv_bad_iter, "Incompatible key-value iterators");
         DCD_ASSERT(!current || !tracker.is_removed(*current), kv_bad_iter, "Iterator to erased element");
         DCD_ASSERT(!r.current || !tracker.is_removed(*r.current), kv_bad_iter, "Iterator to erased element");
         if (!r.current) {
            if (!current)
               return 0;
            else {
               return -1;
            }
         }
         if (!current) {
            return 1;
         }
         
         return compare_blob(current->kv_key, r.current->kv_key);
      }

      int32_t kv_it_key_compare(const char* key, uint32_t size) override {
         if (!current)
            return 1;
         DCD_ASSERT(!tracker.is_removed(*current), kv_bad_iter, "Iterator to erased element");
         return compare_blob(current->kv_key, std::string_view{ key, size });
      }

      kv_it_stat kv_it_move_to_end() override {
         current = nullptr;
         return kv_it_stat::iterator_end;
      }

      kv_it_stat kv_it_next(uint32_t* found_key_size, uint32_t* found_value_size) override {
         if (current) {
            DCD_ASSERT(!tracker.is_removed(*current), kv_bad_iter, "Iterator to erased element");
            auto it = idx.iterator_to(*current);
            ++it;
            return move_to(it, found_key_size, found_value_size);
         }
         return move_to(idx.lower_bound(boost::make_tuple(contract, prefix)), found_key_size, found_value_size);
      }

      kv_it_stat kv_it_prev(uint32_t* found_key_size, uint32_t* found_value_size) override {
         std::decay_t<decltype(idx.end())> it;
         if (current) {
            DCD_ASSERT(!tracker.is_removed(*current), kv_bad_iter, "Iterator to erased element");
            it = idx.iterator_to(*current);
         } else
            it = idx.upper_bound(boost::make_tuple(contract, blob_prefix{{prefix.data(), prefix.size()}}));
         if (it != idx.begin())
            return move_to(--it, found_key_size, found_value_size);
         current = nullptr;
         *found_key_size = 0;
         *found_value_size = 0;
         return kv_it_stat::iterator_end;
      }

      kv_it_stat kv_it_lower_bound(const char* key, uint32_t size, uint32_t* found_key_size, uint32_t* found_value_size) override {
         auto clamped_key = std::max(std::string_view{ key, size }, std::string_view{ prefix.data(), prefix.size() }, unsigned_blob_less{});
         return move_to(idx.lower_bound(boost::make_tuple(contract, clamped_key)), found_key_size, found_value_size);
      }

      kv_it_stat kv_it_key(uint32_t offset, char* dest, uint32_t size, uint32_t& actual_size) override {
         if (!current) {
            actual_size = 0;
            return kv_it_stat::iterator_end;
         }
         DCD_ASSERT(!tracker.is_removed(*current), kv_bad_iter, "Iterator to erased element");
         if (offset < current->kv_key.size())
            memcpy(dest, current->kv_key.data() + offset, std::min((size_t)size, current->kv_key.size() - offset));
         actual_size = current->kv_key.size();
         return kv_it_stat::iterator_ok;
      }

      kv_it_stat kv_it_value(uint32_t offset, char* dest, uint32_t size, uint32_t& actual_size) override {
         if (!current) {
            actual_size = 0;
            return kv_it_stat::iterator_end;
         }
         DCD_ASSERT(!tracker.is_removed(*current), kv_bad_iter, "Iterator to erased element");
         if (offset < current->kv_value.size())
            memcpy(dest, current->kv_value.data() + offset, std::min((size_t)size, current->kv_value.size() - offset));
         actual_size = current->kv_value.size();
         return kv_it_stat::iterator_ok;
      }

      std::optional<name> kv_it_payer() override {
         if (!current) return {};
         return current->payer;
      }                              
   }; // kv_iterator_chainbase

   template<typename Resource_manager>
   struct kv_context_chainbase : kv_context {
      using tracker_type = std::decay_t<decltype(std::declval<chainbase::database>().get_mutable_index<kv_index>().track_removed())>;
      chainbase::database&         db;
      tracker_type                 tracker = db.get_mutable_index<kv_index>().track_removed();
      name                         receiver;
      Resource_manager             resource_manager;
      const kv_database_config&    limits;
      uint32_t                     num_iterators = 0;
      std::optional<shared_blob>   temp_data_buffer;

      kv_context_chainbase(chainbase::database& db, name receiver,
                           Resource_manager resource_manager, const kv_database_config& limits)
         : db{ db }, receiver{ receiver }, resource_manager{ resource_manager }, limits{limits} {}

      int64_t kv_erase(uint64_t contract, const char* key, uint32_t key_size) override {
         DCD_ASSERT(name{ contract } == receiver, table_operation_not_permitted, "Can not write to this key");
         temp_data_buffer.reset();
         auto* kv = db.find<kv_object, by_kv_key>(boost::make_tuple(name{ contract }, std::string_view{ key, key_size }));
         if (!kv)
            return 0;
         const int64_t resource_delta = erase_table_usage(resource_manager, kv->payer, key, kv->kv_key.size(), kv->kv_value.size());

         if (auto dm_logger = resource_manager._context->control.get_deep_mind_logger()) {
            fc_dlog(*dm_logger, "KV_OP REM ${action_id} ${db} ${payer} ${key} ${odata}",
               ("action_id", resource_manager._context->get_action_id())
               ("contract", name{ contract })
               ("payer", kv->payer)
               ("key", fc::to_hex(kv->kv_key.data(), kv->kv_key.size()))
               ("odata", fc::to_hex(kv->kv_value.data(), kv->kv_value.size()))
            );
         }

         tracker.remove(*kv);
         return resource_delta;
      }

      int64_t kv_set(uint64_t contract, const char* key, uint32_t key_size, const char* value,
                     uint32_t value_size, account_name payer) override {
         DCD_ASSERT(name{ contract } == receiver, table_operation_not_permitted, "Can not write to this key");
         DCD_ASSERT(key_size <= limits.max_key_size, kv_limit_exceeded, "Key too large");
         DCD_ASSERT(value_size <= limits.max_value_size, kv_limit_exceeded, "Value too large");

         temp_data_buffer.reset();
         auto* kv = db.find<kv_object, by_kv_key>(
               boost::make_tuple(name{ contract }, std::string_view{ key, key_size }));
         if (kv) {
            const auto resource_delta = update_table_usage(resource_manager, kv->payer, payer, key, key_size, kv->kv_value.size(), value_size);

            if (auto dm_logger = resource_manager._context->control.get_deep_mind_logger()) {
               fc_dlog(*dm_logger, "KV_OP UPD ${action_id} ${db} ${payer} ${key} ${odata}:${ndata}",
                  ("action_id", resource_manager._context->get_action_id())
                  ("contract", name{ contract })
                  ("payer", payer)
                  ("key", fc::to_hex(kv->kv_key.data(), kv->kv_key.size()))
                  ("odata", fc::to_hex(kv->kv_value.data(), kv->kv_value.size()))
                  ("ndata", fc::to_hex(value, value_size))
               );
            }

            db.modify(*kv, [&](auto& obj) {
               obj.kv_value.assign(value, value_size);
               obj.payer = payer;
            });
            return resource_delta;
         } else {
            const int64_t resource_delta = create_table_usage(resource_manager, payer, key, key_size, value_size);
            db.create<kv_object>([&](auto& obj) {
               obj.contract    = name{ contract };
               obj.kv_key.assign(key, key_size);
               obj.kv_value.assign(value, value_size);
               obj.payer       = payer;
            });

            if (auto dm_logger = resource_manager._context->control.get_deep_mind_logger()) {
               fc_dlog(*dm_logger, "KV_OP INS ${action_id} ${db} ${payer} ${key} ${ndata}",
                  ("action_id", resource_manager._context->get_action_id())
                  ("contract", name{ contract })
                  ("payer", payer)
                  ("key", fc::to_hex(key, key_size))
                  ("ndata", fc::to_hex(value, value_size))
               );
            }

            return resource_delta;
         }
      }

      bool kv_get(uint64_t contract, const char* key, uint32_t key_size, uint32_t& value_size) override {
         auto* kv = db.find<kv_object, by_kv_key>(boost::make_tuple(name{ contract }, std::string_view{ key, key_size }));
         if (kv) {
            temp_data_buffer.emplace(kv->kv_value);
            value_size = temp_data_buffer->size();
            return true;
         } else {
            temp_data_buffer.reset();
            value_size = 0;
            return false;
         }
      }

      uint32_t kv_get_data(uint32_t offset, char* data, uint32_t data_size) override {
         const char* temp      = nullptr;
         uint32_t    temp_size = 0;
         if (temp_data_buffer) {
            temp      = temp_data_buffer->data();
            temp_size = temp_data_buffer->size();
         }
         if (offset < temp_size)
            memcpy(data, temp + offset, std::min(data_size, temp_size - offset));
         return temp_size;
      }

      std::unique_ptr<kv_iterator> kv_it_create(uint64_t contract, const char* prefix, uint32_t size) override {
         DCD_ASSERT(num_iterators < limits.max_iterators, kv_bad_iter, "Too many iterators");
         DCD_ASSERT(size <= limits.max_key_size, kv_bad_iter, "Prefix too large");
         return std::make_unique<kv_iterator_chainbase>(db, tracker, num_iterators, name{ contract },
                                                        std::vector<char>{ prefix, prefix + size });
      }
   }; // kv_context_chainbase

   template<typename Resource_manager>
   std::unique_ptr<kv_context> create_kv_chainbase_context(chainbase::database& db, name receiver,
                                                           Resource_manager resource_manager, const kv_database_config& limits)
   {
      return std::make_unique<kv_context_chainbase<Resource_manager>>(db,receiver, resource_manager, limits);
   }

}} // namespace dcd::chain
