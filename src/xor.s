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
	mov r10, r9;
	mov r11, 0x42	; store text length (replaced when packed)
	mov r12, 0		;

.addrights:

	mov rdi, r9		; addr of text section
	mov rsi, 8192	; size of text section
	mov rdx, 7		; PROT_WRITE
	mov rax, 10 	; sys_mprotect
	syscall

.decrypt:

	cmp r12, r11			; if r10 == r11 (meaning we iterated through every bytes of the text section)
	je .removerights		; continue program execution
	mov bl, 0x2a
	xor byte [r10], bl		; oep[r12] ^= 42	
	inc r12
	inc r10
	jmp .decrypt

.removerights:

	mov rdi, r9		; addr of text section
	mov rsi, r11	; size of text section
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
