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

%macro pushx 1-*
 %rep %0
   push %1
   %rotate 1
 %endrep
%endmacro

%macro popx 1-*
  %rep %0
    %rotate -1
    pop %1
  %endrep
%endmacro

%macro qr 4
	add %1, %2
	xor %4, %1
	rol %4, 16
	add %3, %4
	xor %2, %3
	rol %2, 12
	add %1, %2
	xor %4, %1
	rol %4, 8
	add %3, %4
	xor %2, %3
	rol %2, 7
%endmacro

%macro write 2
	mov rdi, 1
	mov rsi, %1
	mov rdx, %2
	mov rax, 4
	syscall
%endmacro

.reginit:
	pushx rdi, rsi, rdx, rax, r9, r10, r11, r12, r13,r14

.varinit:
	mov r9, 0x42	; oep
	mov r10, r9;	; iterator
	mov r11, 0x42	; text length
	mov r12, 0		; counter
	mov r13, 0x42	; base addr of the first page containing the .text section
	mov r14, 0x42	; len of mrpotects calls

.addrights:
	pushx r10, r11, r12

	mov rdi, r13	;
	mov rsi, r14	;
	mov rdx, 7		; PROT_WRITE | PROT_READ | PROT_EXEC
	mov rax, 10 	; sys_mprotect
	syscall

	popx r10, r11, r12

.decrypt:
	cmp r12, r11	
	je .removerights
	mov rdi, r12
	mov rsi, 64
	call modulo
	cmp rax, 0
	jne .xorop

.mat_copy:
	; do the copy
	mov rdi, 10

.block_loop:
	cmp rdi, 0
	je .mat_add
	
	qr [matcpy + 0], [matcpy + 4], [matcpy + 8], [matcpy + 12]
	;qt
	;qt
	;qt
	;qt
	;qt
	;qt
	;qt

	dec rdi
	jmp .block_loop

.mat_add:
	
.xorop:
	mov bl, [mat + rax] ; risky
	xor [r10], bl
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
	write woody, 5

.regrest:
	popx rdi, rsi, rdx, rax, rax, r10, r11, r12, r13,r14

.return:
	push r9		; jmp to oep (r9 is a scratch register)
	ret

modulo:
.modop:
	cmp rdi, rsi
	jl .ret
	sub rdi, rsi
	jmp .modop
.ret:
	mov rax, rdi
	ret

.matrice:
	mat dd 0x61707865, 0x3320646e, 0x79622d32, 0x6b206574
	key db 0x42, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	dd 0x00, 0x00, 0x42, 0x42
	matcpy dd 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	woody db "WOODY"
