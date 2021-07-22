push rdi
push rsi
push rdx
push rax

mov rax, 0x000a79646f6f57
push rax

mov rdi, 1
mov rsi, rsp
mov rdx, 6
mov rax, 1
syscall

pop rax

pop rax
pop rdx
pop rsi
pop rdi
ret
