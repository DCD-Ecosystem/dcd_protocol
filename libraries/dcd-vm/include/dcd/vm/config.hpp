#pragma once

namespace dcd { namespace vm {

// create constexpr flags for whether the backend should obey alignment hints
#ifdef DCD_VM_ALIGN_MEMORY_OPS
   inline constexpr bool should_align_memory_ops = true;
#else
   inline constexpr bool should_align_memory_ops = false;
#endif


#ifdef DCD_VM_SOFTFLOAT
   inline constexpr bool use_softfloat = true;
#else
   inline constexpr bool use_softfloat = false;
#endif

#ifdef DCD_VM_FULL_DEBUG
   inline constexpr bool dcd_vm_debug = true;
#else
   inline constexpr bool dcd_vm_debug = false;
#endif

}} // namespace dcd::vm
