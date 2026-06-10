global _start
section .data
section .text
_start:
push rbp
mov rbp, rsp
sub rsp, 32
call main
mov rax, 60
syscall
add_two:
mov [rsp-8], rdi
mov [rsp-16], rsi
mov rax, [rsp-16]
add rax, [rsp-8]
mov [rsp-24], rax
mov rax, [rsp-24]
mov [rsp-32], rax
ret
main:
mov rdi, 1
mov rsi, 1
call add_two
mov [rsp-40], rax
mov rax, 2
add rax, [rsp-40]
mov [rsp-48], rax
mov rax, [rsp-48]
add rax, 1
mov [rsp-56], rax
mov rax, [rsp-56]
mov [rsp-64], rax
mov rax, 5
cmp rax, [rsp-64]
jne L1
mov rdi, [rsp-64]
mov rsi, 5
call add_two
mov [rsp-72], rax
mov rax, [rsp-72]
mov [rsp-80], rax
L1:
ret
