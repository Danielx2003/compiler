global _start
section .data
section .text
_start:
push rbp
mov rbp, rsp
sub rsp, 32
mov rax, 2
add rax, 1
mov [rsp-8], rax
mov rax, [rsp-8]
mov [rsp-16], rax
L1:
mov rax, 3
cmp rax, [rsp-16]
jne L2
mov rax, 1
add rax, [rsp-16]
mov [rsp-24], rax
mov rax, [rsp-24]
mov [rsp-16], rax
mov rax, 50
mov [rsp-32], rax
jmp L1
L2:
mov rax, 60
syscall
