#pragma once
#include <dcd/chain/code_object.hpp>
#include <dcd/chain/types.hpp>
#include <dcd/chain/whitelisted_intrinsics.hpp>
#include <dcd/chain/exceptions.hpp>
#include "Runtime/Linker.h"
#include "Runtime/Runtime.h"

namespace dcd { namespace chain {

   class apply_context;
   class wasm_runtime_interface;
   class controller;
   namespace dcdvmoc { struct config; }

   struct wasm_exit {
      int32_t code = 0;
   };

   /**
    * @class wasm_interface
    *
    */
   class wasm_interface {
      public:
         enum class vm_type {
            dcd_vm,
            dcd_vm_jit,
            dcd_vm_oc
         };

         //return string description of vm_type
         static std::string vm_type_string(vm_type vmtype) {
             switch (vmtype) {
             case vm_type::dcd_vm:
                return "dcd-vm";
             case vm_type::dcd_vm_oc:
                return "dcd-vm-oc";
             default:
                 return "dcd-vm-jit";
             }
         }

         wasm_interface(vm_type vm, bool dcdvmoc_tierup, const chainbase::database& d, const boost::filesystem::path data_dir, const dcdvmoc::config& dcdvmoc_config);
         ~wasm_interface();

         //call before dtor to skip what can be minutes of dtor overhead with some runtimes; can cause leaks
         void indicate_shutting_down();

         //validates code -- does a WASM validation pass and checks the wasm against DCD specific constraints
         static void validate(const controller& control, const bytes& code);

         //indicate that a particular code probably won't be used after given block_num
         void code_block_num_last_used(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, const uint32_t& block_num);

         //indicate the current LIB. evicts old cache entries
         void current_lib(const uint32_t lib);

         //Calls apply or error on a given code
         void apply(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, apply_context& context);

         //Immediately exits currently running wasm. UB is called when no wasm running
         void exit();

      private:
         unique_ptr<struct wasm_interface_impl> my;
   };

} } // dcd::chain

namespace dcd{ namespace chain {
   std::istream& operator>>(std::istream& in, wasm_interface::vm_type& runtime);
}}

FC_REFLECT_ENUM( dcd::chain::wasm_interface::vm_type, (dcd_vm)(dcd_vm_jit)(dcd_vm_oc) )
