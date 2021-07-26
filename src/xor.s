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
	push r13
	push r14

.varinit:

	mov r9, 0x42	; oep
	mov r10, r9;	; iterator
	mov r11, 0x42	; text length
	mov r12, 0		; counter
	mov r13, 0x42	; base addr of the first page containing the .text section
	mov r14, 0x42	; len of mrpotects calls

.addrights:

	push r10
	push r11
	push r12

	mov rdi, r13	;
	mov rsi, r14	;
	mov rdx, 7		; PROT_WRITE | PROT_READ | PROT_EXEC
	mov rax, 10 	; sys_mprotect
	syscall

	pop r12
	pop r11
	pop r10

.decrypt:
	
	mov bl, 0x2a
	cmp r12, r11	
	je .removerights
	xor byte [r10], bl
	inc r12
	inc r10
	jmp .decrypt

.removerights:

	mov rdi, r13	;
	mov rsi, r14	;
	mov rdx, 5		; PROT_EXEC | PROT_READ 
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

	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop rax
	pop rax
	pop rdx
	pop rsi
	pop rdi

.return:

	push r9		; jmp to oep (r9 is a scratch register)
	ret
