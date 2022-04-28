.file	"switch_stack_linux.s"
.text
.globl	dcdvmoc_switch_stack
.type	dcdvmoc_switch_stack, @function
dcdvmoc_switch_stack:
   movq %rsp, -16(%rdi)
   leaq -16(%rdi), %rsp
   movq %rdx, %rdi
   callq *%rsi
   mov (%rsp), %rsp
   retq
.size	dcdvmoc_switch_stack, .-dcdvmoc_switch_stack
.section	.note.GNU-stack,"",@progbits
