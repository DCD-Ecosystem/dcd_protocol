#pragma once

#include <dcd/chain/types.hpp>
#include <dcd/chain/webassembly/dcd-vm-oc/dcd-vm-oc.h>

#include <exception>

#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>

#include <vector>
#include <list>

namespace dcd { namespace chain {

class apply_context;

namespace dcdvmoc {

using control_block = dcd_vm_oc_control_block;

struct no_offset{};
struct code_offset {
   size_t offset; 
};    
struct intrinsic_ordinal { 
   size_t ordinal; 
};

using dcdvmoc_optional_offset_or_import_t = std::variant<no_offset, code_offset, intrinsic_ordinal>;

struct code_descriptor {
   digest_type code_hash;
   uint8_t vm_version;
   uint8_t codegen_version;
   size_t code_begin;
   dcdvmoc_optional_offset_or_import_t start;
   unsigned apply_offset;
   int starting_memory_pages;
   size_t initdata_begin;
   unsigned initdata_size;
   unsigned initdata_prologue_size;
};

enum dcdvmoc_exitcode : int {
   DCDVMOC_EXIT_CLEAN_EXIT = 1,
   DCDVMOC_EXIT_CHECKTIME_FAIL,
   DCDVMOC_EXIT_SEGV,
   DCDVMOC_EXIT_EXCEPTION
};

}}}

FC_REFLECT(dcd::chain::dcdvmoc::no_offset, );
FC_REFLECT(dcd::chain::dcdvmoc::code_offset, (offset));
FC_REFLECT(dcd::chain::dcdvmoc::intrinsic_ordinal, (ordinal));
FC_REFLECT(dcd::chain::dcdvmoc::code_descriptor, (code_hash)(vm_version)(codegen_version)(code_begin)(start)(apply_offset)(starting_memory_pages)(initdata_begin)(initdata_size)(initdata_prologue_size));

#define DCDVMOC_INTRINSIC_INIT_PRIORITY __attribute__((init_priority(198)))
