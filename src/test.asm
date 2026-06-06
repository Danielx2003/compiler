global _start
section .data
section .text
_start:
push rbp
mov rbp, rsp
sub rsp, 32
mov rax, 2
add rax, 1
mov [rsp - 0], rax
mov rax, [rsp - 0]
mov [rsp - 8], rax
mov rax, [rsp - 0]
mov [rsp - 16], rax
mov rax, 60
syscall
