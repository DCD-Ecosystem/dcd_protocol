#pragma once

#include <dcd/chain/webassembly/common.hpp>
#include <dcd/chain/webassembly/runtime_interface.hpp>
#include <dcd/chain/exceptions.hpp>
#include <dcd/chain/apply_context.hpp>
#include <dcd/chain/wasm_config.hpp>
#include <dcd/chain/whitelisted_intrinsics.hpp>
#include <softfloat_types.h>

//dcd-vm includes
#include <dcd/vm/backend.hpp>

namespace dcd { namespace chain { namespace webassembly { namespace dcd_vm_runtime {

struct apply_options;

}}

template <typename Impl>
using dcd_vm_backend_t = dcd::vm::backend<dcd_vm_host_functions_t, Impl, webassembly::dcd_vm_runtime::apply_options>;

template <typename Options>
using dcd_vm_null_backend_t = dcd::vm::backend<dcd_vm_host_functions_t, dcd::vm::null_backend, Options>;

namespace webassembly { namespace dcd_vm_runtime {

using namespace fc;
using namespace dcd::vm;

void validate(const bytes& code, const whitelisted_intrinsics_type& intrinsics );

void validate(const bytes& code, const wasm_config& cfg, const whitelisted_intrinsics_type& intrinsics );

struct apply_options;

template<typename Backend>
class dcd_vm_runtime : public dcd::chain::wasm_runtime_interface {
   public:
      dcd_vm_runtime();
      bool inject_module(IR::Module&) override;
      std::unique_ptr<wasm_instantiated_module_interface> instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t>,
                                                                             const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version) override;

      void immediately_exit_currently_running_module() override;

   private:
      // todo: managing this will get more complicated with sync calls;
      //       immediately_exit_currently_running_module() should probably
      //       move from wasm_runtime_interface to wasm_instantiated_module_interface.
      dcd_vm_backend_t<Backend>* _bkend = nullptr;  // non owning pointer to allow for immediate exit

   template<typename Impl>
   friend class dcd_vm_instantiated_module;
};

}}}}// dcd::chain::webassembly::dcd_vm_runtime
