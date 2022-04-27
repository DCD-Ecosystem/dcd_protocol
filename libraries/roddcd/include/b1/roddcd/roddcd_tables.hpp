#pragma once

#include <b1/roddcd/callbacks/kv.hpp>
#include <b1/roddcd/constants.hpp>
#include <dcd/ship_protocol.hpp>
#include <dcd/to_key.hpp>

namespace dcd {
using b1::roddcd::kv_environment;
}

#include <dcd/key_value.hpp>

namespace b1::roddcd {

using account           = dcd::ship_protocol::account;
using account_metadata  = dcd::ship_protocol::account_metadata;
using code              = dcd::ship_protocol::code;
using contract_index128 = dcd::ship_protocol::contract_index128;
using contract_index64  = dcd::ship_protocol::contract_index64;
using contract_row      = dcd::ship_protocol::contract_row;
using contract_table    = dcd::ship_protocol::contract_table;
using global_property   = dcd::ship_protocol::global_property;
using key_value         = dcd::ship_protocol::key_value;
using key_value_v0      = dcd::ship_protocol::key_value_v0;
using producer_schedule = dcd::ship_protocol::producer_schedule;
using table_delta       = dcd::ship_protocol::table_delta;
using table_delta_v1    = dcd::ship_protocol::table_delta_v1;

struct fill_status_v0 {
   dcd::checksum256 chain_id        = {};
   uint32_t           head            = {};
   dcd::checksum256 head_id         = {};
   uint32_t           irreversible    = {};
   dcd::checksum256 irreversible_id = {};
   uint32_t           first           = {};
};

DCD_REFLECT(fill_status_v0, chain_id, head, head_id, irreversible, irreversible_id, first)

using fill_status = std::variant<fill_status_v0>;

inline bool operator==(const fill_status_v0& a, fill_status_v0& b) {
   return std::tie(a.head, a.head_id, a.irreversible, a.irreversible_id, a.first) ==
          std::tie(b.head, b.head_id, b.irreversible, b.irreversible_id, b.first);
}

inline bool operator!=(const fill_status_v0& a, fill_status_v0& b) { return !(a == b); }

using fill_status_sing = dcd::kv_singleton<fill_status, dcd::name{ "fill.status" }, state_database>;

struct block_info_v0 {
   uint32_t                         num                = {};
   dcd::checksum256               id                 = {};
   dcd::block_timestamp           timestamp          = {};
   dcd::name                      producer           = {};
   uint16_t                         confirmed          = {};
   dcd::checksum256               previous           = {};
   dcd::checksum256               transaction_mroot  = {};
   dcd::checksum256               action_mroot       = {};
   uint32_t                         schedule_version   = {};
   std::optional<producer_schedule> new_producers      = {};
   dcd::signature                 producer_signature = {};
};

DCD_REFLECT(block_info_v0, num, id, timestamp, producer, confirmed, previous, transaction_mroot, action_mroot,
              schedule_version, new_producers, producer_signature)

using block_info = std::variant<block_info_v0>;

// todo: move out of "state"?
struct block_info_kv : dcd::kv_table<block_info> {
   index<uint32_t> primary_index{ dcd::name{ "primary" }, [](const auto& var) {
                                    return std::visit([](const auto& obj) { return obj.num; }, *var);
                                 } };

   index<dcd::checksum256> id_index{ dcd::name{ "id" }, [](const auto& var) {
                                         return std::visit([](const auto& obj) { return obj.id; }, *var);
                                      } };

   block_info_kv(dcd::kv_environment environment) : dcd::kv_table<block_info>{ std::move(environment) } {
      init(state_account, dcd::name{ "block.info" }, state_database, primary_index, id_index);
   }
};

struct global_property_kv : dcd::kv_table<global_property> {
   index<std::vector<char>> primary_index{ dcd::name{ "primary" },
                                           [](const auto& var) { return std::vector<char>{}; } };

   global_property_kv(dcd::kv_environment environment) : dcd::kv_table<global_property>{ std::move(environment) } {
      init(state_account, dcd::name{ "global.prop" }, state_database, primary_index);
   }
};

struct account_kv : dcd::kv_table<account> {
   index<dcd::name> primary_index{ dcd::name{ "primary" }, [](const auto& var) {
                                       return std::visit([](const auto& obj) { return obj.name; }, *var);
                                    } };

   account_kv(dcd::kv_environment environment) : dcd::kv_table<account>{ std::move(environment) } {
      init(state_account, dcd::name{ "account" }, state_database, primary_index);
   }
};

struct account_metadata_kv : dcd::kv_table<account_metadata> {
   index<dcd::name> primary_index{ dcd::name{ "primary" }, [](const auto& var) {
                                       return std::visit([](const auto& obj) { return obj.name; }, *var);
                                    } };

   account_metadata_kv(dcd::kv_environment environment)
       : dcd::kv_table<account_metadata>{ std::move(environment) } {
      init(state_account, dcd::name{ "account.meta" }, state_database, primary_index);
   }
};

struct code_kv : dcd::kv_table<code> {
   index<std::tuple<const uint8_t&, const uint8_t&, const dcd::checksum256&>> primary_index{
      dcd::name{ "primary" },
      [](const auto& var) {
         return std::visit([](const auto& obj) { return std::tie(obj.vm_type, obj.vm_version, obj.code_hash); }, *var);
      }
   };

   code_kv(dcd::kv_environment environment) : dcd::kv_table<code>{ std::move(environment) } {
      init(state_account, dcd::name{ "code" }, state_database, primary_index);
   }
};

struct contract_table_kv : dcd::kv_table<contract_table> {
   index<std::tuple<const dcd::name&, const dcd::name&, const dcd::name&>> primary_index{
      dcd::name{ "primary" },
      [](const auto& var) {
         return std::visit([](const auto& obj) { return std::tie(obj.code, obj.table, obj.scope); }, *var);
      }
   };

   contract_table_kv(dcd::kv_environment environment) : dcd::kv_table<contract_table>{ std::move(environment) } {
      init(state_account, dcd::name{ "contract.tab" }, state_database, primary_index);
   }
};

struct contract_row_kv : dcd::kv_table<contract_row> {
   using PT = typename std::tuple<const dcd::name&, const dcd::name&, const dcd::name&, const uint64_t&>;
   index<PT> primary_index{ dcd::name{ "primary" }, [](const auto& var) {
                              return std::visit(
                                    [](const auto& obj) {
                                       return std::tie(obj.code, obj.table, obj.scope, obj.primary_key);
                                    },
                                    *var);
                           } };

   contract_row_kv(dcd::kv_environment environment) : dcd::kv_table<contract_row>{ std::move(environment) } {
      init(state_account, dcd::name{ "contract.row" }, state_database, primary_index);
   }
};

struct contract_index64_kv : dcd::kv_table<contract_index64> {
   using PT = typename std::tuple<const dcd::name&, const dcd::name&, const dcd::name&, const uint64_t&>;
   index<PT> primary_index{ dcd::name{ "primary" }, [](const auto& var) {
                              return std::visit(
                                    [](const auto& obj) {
                                       return std::tie(obj.code, obj.table, obj.scope, obj.primary_key);
                                    },
                                    *var);
                           } };
   using ST = typename std::tuple<const dcd::name&, const dcd::name&, const dcd::name&, const uint64_t&,
                                  const uint64_t&>;
   index<ST> secondary_index{ dcd::name{ "secondary" }, [](const auto& var) {
                                return std::visit(
                                      [](const auto& obj) {
                                         return std::tie(obj.code, obj.table, obj.scope, obj.secondary_key,
                                                         obj.primary_key);
                                      },
                                      *var);
                             } };

   contract_index64_kv(dcd::kv_environment environment)
       : dcd::kv_table<contract_index64>{ std::move(environment) } {
      init(state_account, dcd::name{ "contract.i1" }, state_database, primary_index, secondary_index);
   }
};

struct contract_index128_kv : dcd::kv_table<contract_index128> {
   using PT = typename std::tuple<const dcd::name&, const dcd::name&, const dcd::name&, const uint64_t&>;
   index<PT> primary_index{ dcd::name{ "primary" }, [](const auto& var) {
                              return std::visit(
                                    [](const auto& obj) {
                                       return std::tie(obj.code, obj.table, obj.scope, obj.primary_key);
                                    },
                                    *var);
                           } };
   using ST = typename std::tuple<const dcd::name&, const dcd::name&, const dcd::name&, const __uint128_t&,
                                  const uint64_t&>;
   index<ST> secondary_index{ dcd::name{ "secondary" }, [](const auto& var) {
                                return std::visit(
                                      [](const auto& obj) {
                                         return std::tie(obj.code, obj.table, obj.scope, obj.secondary_key,
                                                         obj.primary_key);
                                      },
                                      *var);
                             } };

   contract_index128_kv(dcd::kv_environment environment)
       : dcd::kv_table<contract_index128>{ std::move(environment) } {
      init(state_account, dcd::name{ "contract.i2" }, state_database, primary_index, secondary_index);
   }
};

template <typename Table, typename D, typename F>
void store_delta_typed(dcd::kv_environment environment, D& delta, bool bypass_preexist_check, F f) {
   Table table{ environment };
   for (auto& row : delta.rows) {
      f();
      auto obj = dcd::from_bin<typename Table::value_type>(row.data);
      if (row.present)
         table.put(obj);
      else
         table.erase(obj);
   }
}

template <typename D, typename F>
void store_delta_kv(dcd::kv_environment environment, D& delta, F f) {
   for (auto& row : delta.rows) {
      f();
      auto  obj  = dcd::from_bin<key_value>(row.data);
      auto& obj0 = std::get<key_value_v0>(obj);
#warning uncomment this when we remove the database logic on roddcd
      /*
      if (row.present)
         environment.kv_set(obj0.database.value, obj0.contract.value, obj0.key.pos, obj0.key.remaining(),
                            obj0.value.pos, obj0.value.remaining());
      else
         environment.kv_erase(obj0.database.value, obj0.contract.value, obj0.key.pos, obj0.key.remaining());
      */
   }
}

template <typename D, typename F>
inline void store_delta(dcd::kv_environment environment, D& delta, bool bypass_preexist_check, F f) {
   if (delta.name == "global_property")
      store_delta_typed<global_property_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "account")
      store_delta_typed<account_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "account_metadata")
      store_delta_typed<account_metadata_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "code")
      store_delta_typed<code_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "contract_table")
      store_delta_typed<contract_table_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "contract_row")
      store_delta_typed<contract_row_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "contract_index64")
      store_delta_typed<contract_index64_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "contract_index128")
      store_delta_typed<contract_index128_kv>(environment, delta, bypass_preexist_check, f);
   if (delta.name == "key_value")
      store_delta_kv(environment, delta, f);
}

inline void store_deltas(dcd::kv_environment environment, std::vector<table_delta>& deltas,
                         bool bypass_preexist_check) {
   for (auto& delta : deltas) //
      std::visit([&](auto& delta_any_v) { store_delta(environment, delta_any_v, bypass_preexist_check, [] {}); }, delta);
}

} // namespace b1::roddcd
