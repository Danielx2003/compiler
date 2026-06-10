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
mov rax, [rsp-16]
add rax, [rsp-8]
mov [rsp-24], rax
mov rax, [rsp-24]
ret
main:
mov rax, 1
mov [rsp-32], rax
mov rax, 11
mov [rsp-40], rax
mov rdi, [rsp-32]
mov rsi, [rsp-40]
call add_two
mov [rsp-48], rax
mov rax, 5
add rax, [rsp-48]
mov [rsp-56], rax
mov rax, [rsp-56]
add rax, 5
mov [rsp-64], rax
mov rax, [rsp-64]
ret
