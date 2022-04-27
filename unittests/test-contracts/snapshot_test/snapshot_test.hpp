#pragma once

#include <dcd/dcd.hpp>

class [[dcd::contract]] snapshot_test : public dcd::contract {
public:
   using dcd::contract::contract;

   [[dcd::action]]
   void increment( uint32_t value );

   struct [[dcd::table("data")]] main_record {
      uint64_t           id         = 0;
      double             index_f64  = 0.0;
      long double        index_f128 = 0.0L;
      uint64_t           index_i64  = 0ULL;
      uint128_t          index_i128 = 0ULL;
      dcd::checksum256 index_i256;

      uint64_t                  primary_key()const    { return id; }
      double                    get_index_f64()const  { return index_f64 ; }
      long double               get_index_f128()const { return index_f128; }
      uint64_t                  get_index_i64()const  { return index_i64 ; }
      uint128_t                 get_index_i128()const { return index_i128; }
      const dcd::checksum256& get_index_i256()const { return index_i256; }

      DCDLIB_SERIALIZE( main_record, (id)(index_f64)(index_f128)(index_i64)(index_i128)(index_i256) )
   };

   using data_table = dcd::multi_index<"data"_n, main_record,
      dcd::indexed_by< "byf"_n,    dcd::const_mem_fun< main_record, double,
                                                           &main_record::get_index_f64 > >,
      dcd::indexed_by< "byff"_n,   dcd::const_mem_fun< main_record, long double,
                                                           &main_record::get_index_f128> >,
      dcd::indexed_by< "byi"_n,    dcd::const_mem_fun< main_record, uint64_t,
                                                           &main_record::get_index_i64 > >,
      dcd::indexed_by< "byii"_n,   dcd::const_mem_fun< main_record, uint128_t,
                                                           &main_record::get_index_i128 > >,
      dcd::indexed_by< "byiiii"_n, dcd::const_mem_fun< main_record, const dcd::checksum256&,
                                                           &main_record::get_index_i256 > >
   >;
};
