.data
.global main
format:.string "%d"
main:
  movl $16, %edi
  call malloc
  pushq %rax
  
  movq $8, -8(%rsp)
  movq -8(%rsp), %rsi
  movq $format, %rdi
  call printf
  ret
