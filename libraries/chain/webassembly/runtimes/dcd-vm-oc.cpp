#include <dcd/chain/webassembly/dcd-vm-oc.hpp>
#include <dcd/chain/wasm_dcd_constraints.hpp>
#include <dcd/chain/wasm_dcd_injection.hpp>
#include <dcd/chain/apply_context.hpp>
#include <dcd/chain/exceptions.hpp>
#include <dcd/chain/global_property_object.hpp>

#include <vector>
#include <iterator>

namespace dcd { namespace chain { namespace webassembly { namespace dcdvmoc {

class dcdvmoc_instantiated_module : public wasm_instantiated_module_interface {
   public:
      dcdvmoc_instantiated_module(const digest_type& code_hash, const uint8_t& vm_version, dcdvmoc_runtime& wr) :
         _code_hash(code_hash),
         _vm_version(vm_version),
         _dcdvmoc_runtime(wr)
      {

      }

      ~dcdvmoc_instantiated_module() {
         _dcdvmoc_runtime.cc.free_code(_code_hash, _vm_version);
      }

      void apply(apply_context& context) override {
         const code_descriptor* const cd = _dcdvmoc_runtime.cc.get_descriptor_for_code_sync(_code_hash, _vm_version);
         DCD_ASSERT(cd, wasm_execution_error, "DCD VM OC instantiation failed");

         _dcdvmoc_runtime.exec.execute(*cd, _dcdvmoc_runtime.mem, context);
      }

      const digest_type              _code_hash;
      const uint8_t                  _vm_version;
      dcdvmoc_runtime&               _dcdvmoc_runtime;
};

dcdvmoc_runtime::dcdvmoc_runtime(const boost::filesystem::path data_dir, const dcdvmoc::config& dcdvmoc_config, const chainbase::database& db)
   : cc(data_dir, dcdvmoc_config, db), exec(cc), mem(wasm_constraints::maximum_linear_memory/wasm_constraints::wasm_page_size) {
}

dcdvmoc_runtime::~dcdvmoc_runtime() {
}

std::unique_ptr<wasm_instantiated_module_interface> dcdvmoc_runtime::instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t> initial_memory,
                                                                                        const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version) {

   return std::make_unique<dcdvmoc_instantiated_module>(code_hash, vm_type, *this);
}

//never called. DCD VM OC overrides dcd_exit to its own implementation
void dcdvmoc_runtime::immediately_exit_currently_running_module() {}

}}}}
