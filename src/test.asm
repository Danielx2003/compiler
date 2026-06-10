global _start
section .data
section .text
_start:
push rbp
mov rbp, rsp
sub rsp, 32
call main
mov rdi, rax
mov rax, 60
syscall
add_two:
mov [rsp-8], rdi
mov [rsp-16], rsi
