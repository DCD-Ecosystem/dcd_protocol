#include <dcd/dcd.hpp>
#include <dcd/table.hpp>

using namespace dcd;

class [[dcd::contract]] kv_table_test : public dcd::contract {
public:
   struct my_struct {
      dcd::name primary_key;
      uint64_t foo;
      std::string bar;
      uint32_t u32;
      int32_t i32;
      int64_t i64;
      double  f64;

      bool operator==(const my_struct& b) const {
         return primary_key == b.primary_key &&
                foo == b.foo &&
                bar == b.bar &&
                u32 == b.u32 &&
                i32 == b.i32 &&
                i64 == b.i64 &&
                f64 == b.f64;
      }
   };

   struct [[dcd::table]] my_table : dcd::kv::table<my_struct, "kvtable"_n> {
      KV_NAMED_INDEX("primarykey"_n, primary_key)
      KV_NAMED_INDEX("foo"_n, foo)
      KV_NAMED_INDEX("bar"_n, bar)
      KV_NAMED_INDEX("u"_n, u32)
      KV_NAMED_INDEX("i"_n, i32)
      KV_NAMED_INDEX("ii"_n, i64)
      KV_NAMED_INDEX("ff"_n, f64)

      my_table(dcd::name contract_name) {
         init(contract_name, primary_key, foo, bar, u32, i32, i64, f64);
      }
   };

   using contract::contract;

   [[dcd::action]]
   void setup() {
      my_table t{"kvtable"_n};
      char base[5] = {'b', 'o', 'b', 'a', 0};
   
      for (int i = 0; i < 10; ++i) {
         my_struct s;
         s.primary_key = dcd::name{base};
         s.foo = i + 1;
         s.bar = base;
         s.u32 = (i + 1) * 10;
         s.i32 = (i + 1 ) * 10 - 50;
         s.f64 = i + i / 100.0;
         s.i64 = (i + 1) * 100 - 500;
         
         t.put(s, get_self());

         ++base[3];
      }
      verify();
   }

   [[dcd::action]]
   void verify() {
      my_table t{"kvtable"_n};

      auto val = t.primary_key.get("boba"_n);
      dcd::check(val->primary_key == "boba"_n, "Got the wrong primary value");
      dcd::check(val->foo == 1, "Got the wrong foo value");
      dcd::check(val->bar == "boba", "Got the wrong bar value");

      val = t.primary_key.get("bobj"_n);
      dcd::check(val->primary_key == "bobj"_n, "Got the wrong primary value");
      dcd::check(val->foo == 10, "Got the wrong foo value");
      dcd::check(val->bar == "bobj", "Got the wrong bar value");

      val = t.bar.get("boba");
      dcd::check(val->primary_key == "boba"_n, "Got the wrong primary value");
      dcd::check(val->foo == 1, "Got the wrong foo value");
      dcd::check(val->bar == "boba", "Got the wrong bar value");

      val = t.bar.get("bobj");
      dcd::check(val->primary_key == "bobj"_n, "Got the wrong primary value");
      dcd::check(val->foo == 10, "Got the wrong foo value");
      dcd::check(val->bar == "bobj", "Got the wrong bar value");
     
      val = t.foo.get(1);
      dcd::check(val->primary_key == "boba"_n, "Got the wrong primary value");
      dcd::check(val->foo == 1, "Got the wrong foo value");
      dcd::check(val->bar == "boba", "Got the wrong bar value");

      val = t.foo.get(10);
      dcd::check(val->primary_key == "bobj"_n, "Got the wrong primary value");
      dcd::check(val->foo == 10, "Got the wrong foo value");
      dcd::check(val->bar == "bobj", "Got the wrong bar value");
   }
};
