#include <dcd/abi.hpp>
#include "abidcd.hpp"

using namespace dcd;

namespace {

template <int i>
bool ends_with(const std::string& s, const char (&suffix)[i]) {
    return s.size() >= i - 1 && !strcmp(s.c_str() + s.size() - (i - 1), suffix);
}

template <typename T>
struct abi_serializer_impl : abi_serializer {
    void json_to_bin(::abidcd::jvalue_to_bin_state& state, bool allow_extensions, const abi_type* type,
                             bool start) const override {
        return ::abidcd::json_to_bin((T*)nullptr, state, allow_extensions, type, start);
    }
    void json_to_bin(::abidcd::json_to_bin_state& state, bool allow_extensions, const abi_type* type,
                             bool start) const override {
        return ::abidcd::json_to_bin((T*)nullptr, state, allow_extensions, type, start);
    }
    void bin_to_json(::abidcd::bin_to_json_state& state, bool allow_extensions, const abi_type* type,
                             bool start) const override {
        return ::abidcd::bin_to_json((T*)nullptr, state, allow_extensions, type, start);
    }
};

template <typename T>
constexpr auto abi_serializer_for = abi_serializer_impl<T>{};

abi_type::alias resolve(std::map<std::string, abi_type>& abi_types, const abi_type::alias_def* type, int depth);

template<typename... T, typename... A>
bool holds_any_alternative(const std::variant<A...>& v) {
   return (... || std::holds_alternative<T>(v));
}

template <typename F>
constexpr void for_each_abi_type(F f) {
    static_assert(sizeof(float) == 4);
    static_assert(sizeof(double) == 8);
    std::apply([&f](auto&& ...t) { (f(&t), ...); }, basic_abi_types{});
}

abi_type* get_type(std::map<std::string, abi_type>& abi_types,
                           const std::string& name, int depth) {
   dcd::check(depth < 32,
        dcd::convert_abi_error(abi_error::recursion_limit_reached));
    auto it = abi_types.find(name);
    if (it == abi_types.end()) {
        if (ends_with(name, "?")) {
            auto base = get_type(abi_types, name.substr(0, name.size() - 1), depth + 1);
            dcd::check(!holds_any_alternative<abi_type::optional, abi_type::array, abi_type::extension>(base->_data),
                  dcd::convert_abi_error(abi_error::invalid_nesting));
            auto [iter, success] = abi_types.try_emplace(name, name, abi_type::optional{base}, &abi_serializer_for< ::abidcd::pseudo_optional>);
            return &iter->second;
        } else if (ends_with(name, "[]")) {
            auto element = get_type(abi_types, name.substr(0, name.size() - 2), depth + 1);
            dcd::check(!holds_any_alternative<abi_type::optional, abi_type::array, abi_type::extension>(element->_data),
                  dcd::convert_abi_error(abi_error::invalid_nesting));
            auto [iter, success] = abi_types.try_emplace(name, name, abi_type::array{element}, &abi_serializer_for< ::abidcd::pseudo_array>);
            return &iter->second;
        } else if (ends_with(name, "$")) {
            auto base = get_type(abi_types, name.substr(0, name.size() - 1), depth + 1);
            dcd::check(!std::holds_alternative<abi_type::extension>(base->_data),
                  dcd::convert_abi_error(abi_error::invalid_nesting));
            auto [iter, success] = abi_types.try_emplace(name, name, abi_type::extension{base}, &abi_serializer_for< ::abidcd::pseudo_extension>);
            return &iter->second;
        } else
           dcd::check(false, dcd::convert_abi_error(abi_error::unknown_type));
    }

    // resolve aliases
    if (auto* alias = std::get_if<abi_type::alias>(&it->second._data)) {
        return alias->type;
    } else if(auto* alias = std::get_if<const abi_type::alias_def*>(&it->second._data)) {
        auto base = resolve(abi_types, *alias, depth);
        it->second._data = base;
        return base.type;
    }

    return &it->second;
}

abi_type::struct_ resolve(std::map<std::string, abi_type>& abi_types, const struct_def* type, int depth) {
   dcd::check(depth < 32,
        dcd::convert_abi_error(abi_error::recursion_limit_reached));
    abi_type::struct_ result;
    if (!type->base.empty()) {
        auto base = get_type(abi_types, type->base, depth + 1);

        if(auto* base_def = std::get_if<const struct_def*>(&base->_data)) {
            auto b = resolve(abi_types, *base_def, depth + 1);
            base->_data = std::move(b);
        }
        if(auto* b = std::get_if<abi_type::struct_>(&base->_data)) {
            result.fields = b->fields;
        } else {
           dcd::check(false, dcd::convert_abi_error(abi_error::base_not_a_struct));
        }
    }
    for (auto& field : type->fields) {
        auto t = get_type(abi_types, field.type, depth + 1);
        result.fields.push_back(abi_field{field.name, t});
    }
    return result;
}


abi_type::variant resolve(std::map<std::string, abi_type>& abi_types, const variant_def* type, int depth) {
   dcd::check(depth < 32,
        dcd::convert_abi_error(abi_error::recursion_limit_reached));
    abi_type::variant result;
    for (const std::string& field : type->types) {
        auto t = get_type(abi_types, field, depth + 1);
        result.push_back({field, t});
    }
    return result;
}

abi_type::alias resolve(std::map<std::string, abi_type>& abi_types, const abi_type::alias_def* type, int depth) {
    auto t = get_type(abi_types, *type, depth + 1);
    dcd::check(!std::holds_alternative<abi_type::extension>(t->_data),
        dcd::convert_abi_error(abi_error::extension_typedef));
    return abi_type::alias{t};
}

struct fill_t {
   std::map<std::string, abi_type>& abi_types;
   abi_type& type;
   int depth;
   template<typename T>
   auto operator()(T& t) -> std::void_t<decltype(resolve(abi_types, t, depth))> {
      auto x = resolve(abi_types, t, depth);
      type._data = std::move(x);
   }
   template<typename T>
   auto operator()(const T& t) {
   }
};

void fill(std::map<std::string, abi_type>& abi_types, abi_type& type, int depth) {
   return std::visit(fill_t{abi_types, type, depth}, type._data);
}

}


const abi_type* dcd::abi::get_type(const std::string& name) {
   return ::get_type(abi_types, name, 0);
}

void dcd::convert(const abi_def& abi, dcd::abi& c) {
    for (auto& a : abi.actions)
        c.action_types[a.name] = a.type;
    for (auto& t : abi.tables)
        c.table_types[t.name] = t.type;
    for (auto& r : abi.action_results.value)
        c.action_result_types[r.name] = r.result_type;
    for_each_abi_type([&](auto* p) {
        const char* name = get_type_name(p);
        c.abi_types.try_emplace(name, name, abi_type::builtin{}, &abi_serializer_for<std::decay_t<decltype(*p)>>);
    });
    {
        c.abi_types.try_emplace("extended_asset", "extended_asset",
                                abi_type::struct_{nullptr, {{"quantity", &c.abi_types.find("asset")->second},
                                                            {"contract", &c.abi_types.find("name")->second}}},
                                &abi_serializer_for<::abidcd::pseudo_object>);
    }

    for (auto& t : abi.types) {
       dcd::check(!t.new_type_name.empty(),
            dcd::convert_abi_error(abi_error::missing_name));
        auto [_, inserted] = c.abi_types.try_emplace(t.new_type_name, t.new_type_name, &t.type, nullptr);
        dcd::check(inserted,
            dcd::convert_abi_error(abi_error::redefined_type));
    }
    for (auto& s : abi.structs) {
       dcd::check(!s.name.empty(),
            dcd::convert_abi_error(abi_error::missing_name));
        auto [it, inserted] = c.abi_types.try_emplace(s.name, s.name, &s, &abi_serializer_for<::abidcd::pseudo_object>);
        dcd::check(inserted,
            dcd::convert_abi_error(abi_error::redefined_type));
    }
    for (auto& v : abi.variants.value) {
       dcd::check(!v.name.empty(),
            dcd::convert_abi_error(abi_error::missing_name));
        auto [it, inserted] = c.abi_types.try_emplace(v.name, v.name, &v, &abi_serializer_for<::abidcd::pseudo_variant>);
        dcd::check(inserted,
            dcd::convert_abi_error(abi_error::redefined_type));
    }
    for (auto& [_, t] : c.abi_types) {
        fill(c.abi_types, t, 0);
    }

    for (const auto& [key, val] : abi.kv_tables.value) {
        std::vector<char> bytes;
        dcd::vector_stream strm(bytes);
        to_json(val, strm);
        c.kv_tables.try_emplace(key, bytes.begin(), bytes.end());
    }
}

void to_abi_def(abi_def& def, const std::string& name, const abi_type::builtin&) {}
void to_abi_def(abi_def& def, const std::string& name, const abi_type::optional&) {}
void to_abi_def(abi_def& def, const std::string& name, const abi_type::array&) {}
void to_abi_def(abi_def& def, const std::string& name, const abi_type::extension&) {}

template<typename T>
void to_abi_def(abi_def& def, const std::string& name, const T*) {
   dcd::check(false, dcd::convert_abi_error(dcd::abi_error::bad_abi));
}

void to_abi_def(abi_def& def, const std::string& name, const abi_type::alias& alias) {
   def.types.push_back({name, alias.type->name});
}

void to_abi_def(abi_def& def, const std::string& name, const abi_type::struct_& struct_) {
   if(name == "extended_asset") return;
   std::size_t field_offset = 0;
   std::string base;
   std::vector<field_def> fields;
   if(struct_.base) {
      field_offset = struct_.base->as_struct()->fields.size();
      base = struct_.base->name;
   }
   for(std::size_t i = field_offset; i < struct_.fields.size(); ++i) {
      const abi_field& field = struct_.fields[i];
      fields.push_back({field.name, field.type->name});
   }
   def.structs.push_back({name, std::move(base), std::move(fields)});
}

void to_abi_def(abi_def& def, const std::string& name, const abi_type::variant& variant) {
   std::vector<std::string> types;
   for(const auto& [name, type] : variant) {
      types.push_back(type->name);
   }
   def.variants.value.push_back({name, std::move(types)});
}

void dcd::convert(const dcd::abi& abi, dcd::abi_def& def) {
   def.version = "eosio::abi/1.0";
   for(auto& [name, type] : abi.abi_types) {
      std::visit([&name = type.name, &def](const auto& t){ return to_abi_def(def, name, t); }, type._data);
   }
}

const abi_serializer* const dcd::object_abi_serializer = &abi_serializer_for< ::abidcd::pseudo_object>;
const abi_serializer* const dcd::variant_abi_serializer = &abi_serializer_for< ::abidcd::pseudo_variant>;
const abi_serializer* const dcd::array_abi_serializer = &abi_serializer_for< ::abidcd::pseudo_array>;
const abi_serializer* const dcd::extension_abi_serializer = &abi_serializer_for< ::abidcd::pseudo_extension>;
const abi_serializer* const dcd::optional_abi_serializer = &abi_serializer_for< ::abidcd::pseudo_optional>;

std::vector<char> dcd::abi_type::json_to_bin_reorderable(std::string_view json, std::function<void()> f) const {
   abidcd::jvalue tmp;
   abidcd::json_to_jvalue(tmp, json, f);
   std::vector<char> result;
   abidcd::json_to_bin(result, this, tmp, f);
   return result;
}

std::vector<char> dcd::abi_type::json_to_bin(std::string_view json, std::function<void()> f) const {
   std::vector<char> result;
   abidcd::json_to_bin(result, this, json, f);
   return result;
}

std::string dcd::abi_type::bin_to_json(input_stream& bin, std::function<void()> f) const {
   std::string result;
   abidcd::bin_to_json(bin, this, result, f);
   return result;
}