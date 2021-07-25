; **************************************************************************** ;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    stub.s                                             :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2021/07/21 19:41:02 by bccyv             #+#    #+#              ;
;    Updated: 2021/07/24 17:46:26 by bccyv            ###   ########.fr        ;
;                                                                              ;
; **************************************************************************** ;

.reginit:

	push rdi
	push rsi
	push rdx
	push rax
	push r9
	push r10
	push r11
	push r12

.varinit:

	mov r9, 0x42	; store oep (replaced when packed)
	mov r10, 0x42	; store text length (replaced when packed)
	mov r11, r9		;
	mov r12, 0		;

.addrights:

	mov rdi, r9		; addr of text section
	mov rsi, r10	; size of text section
	mov rdx, 2		; PROT_WRITE
	mov rax, 10 	; sys_mprotect
	syscall

.decrypt:

	.pushkey:

		push 0x42
		push 0x42
		push 0x00
		push 0x00

		push 0x42
		push 0x42
		push 0x42
		push 0x42
		push 0x42
		push 0x42
		push 0x42
		push 0x42

		push 0x6b206574
		push 0x79622d32
		push 0x3320646e
		push 0x61707865

	.loop:

		cmp r12, [r10]	; if r10 == 0 (meaning we iterated through every bytes of the text section)
		je .popkey		; continue program execution
	
		mov rdi, [r12]
		mov rsi, 64
		call modulo
		cmp rax, 0
		jne .xorop

		; inc blockcount
		
	.chachablock:

		
	
	.xorop:
		
		mov rdi, r12
		mov rsi, 64
		call modulo
		xor [r9 + r12], [ + rax]

		add r12, 1
		jmp .loop

	.popkey:

		pop rdi
		pop rdi
		pop rdi
		pop rdi

		pop rdi
		pop rdi
		pop rdi
		pop rdi
		pop rdi
		pop rdi
		pop rdi
		pop rdi

		pop rdi
		pop rdi
		pop rdi
		pop rdi

.removerights:

	mov rdi, r9		; addr of text section
	mov rsi, r10	; size of text section
	mov rdx, 4		; PROT_EXEC
	mov rax, 10		; sys_mprotect
	syscall

.print:

	mov rax, 0xa79646f6f57
	push rax

	mov rdi, 1		; STDOUT
	mov rsi, rsp	; addr of "WOODY\n"
	mov rdx, 6		; size of "WOODY\n"
	mov rax, 1		; write
	syscall

	pop rax

.regrest:

	pop r12
	pop r11
	pop r10
	pop rax			; scratch register, to keep r9 to return
	pop rax
	pop rdx
	pop rsi
	pop rdi

.return:

	push r9			; address of OEP
	ret

modulo:
.modloop:
	cmp rdi, rsi
	jl .modret
	sub rdi, [rsi]
.modret:
	mov rax, rdi
	ret
