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

%macro qrop 4
	add %1, %2
	xor %3, %1
	rol %3, %4
%endmacro

%macro qr 5
	mov rdi, [%1 + %2]
	mov rsi, [%1 + %3]
	mov rdx, [%1 + %4]
	mov rcx, [%1 + %5]
	qrop rdi, rsi, rcx, 16
	qrop rdx, rcx, rsi, 12
	qrop rdi, rsi, rcx, 8
	qrop rdx, rcx, rsi, 7
%endmacro

;##############################################################################

stub:

.init:
	pushx rdi, rsi, rdx, rax, r8, r9, r10, r11

	mov r8,  0x42	; oep
	mov r9,  0x42	; text length
	mov r10, 0x42	; base addr of the first page containing the .text section
	mov r11, 0x42	; len of mrpotects calls

.addrights:
	pushx r8, r9, r10, r11

	mov rdi, r10	;
	mov rsi, r11	;
	mov rdx, 7		; PROT_WRITE | PROT_READ | PROT_EXEC
	mov rax, 10 	; sys_mprotect
	syscall

	popx r8, r9, r10, r11

.decrypt:
	mov rdi, r8		; oep
	mov rsi, r9		; len

	call chacha20_decrypt
	
.removerights:
	mov rdi, r10	;
	mov rsi, r11	;
	mov rdx, 5		; PROT_EXEC | PROT_READ 
	mov rax, 10		; sys_mprotect
	syscall

.print:
	mov rdi, 1		; stdout
	mov rsi, woody	;
	mov rdx, 5		; len
	mov rax, 4		; write
	syscall			;

.fini:
	popx rdi, rsi, rdx, rax, r8, rax, r10, r11

	push r9			; jmp to oep (r9 is a scratch register)
	ret

;##############################################################################

chacha20_decrypt:

.init:
	pushx r8, r9, r10, r11, rdx

	mov r8,	0		; iterator through text bytes
	mov r9, rsi		; iterator through text length

.run:
	cmp r9, rdi		; if r9 == length
	je .fini		;
	call mod64		; if r9 % 64
	cmp rax, 0		; == 0;
	jne .xor		;

.mat_copy:

	mov r10b, byte mat
	mov r11b, byte cpy
	mov rdx, 0
	.cpystart:
	cmp rdx, 32
	je .cpyend
	mov [r11], r10
	inc r10
	inc r11
	inc rdx
	.cpyend:
	mov rdx, 10

.block:
	cmp rdx, 0		; iterates 10 times
	je .mat_add
	
	qr cpy, 0, 4, 8, 12
	qr cpy, 1, 5, 9, 13
	qr cpy, 2, 6, 10, 14
	qr cpy, 3, 7, 11, 15
	qr cpy, 0, 5, 10, 15
	qr cpy, 1, 6, 11, 12
	qr cpy, 2, 7, 8, 13
	qr cpy, 3, 4, 9, 14

	dec rdx
	jmp .block

.mat_add:
	mov r10b, byte mat
	mov r11b, byte cpy
	mov rdx, 0
	.addstart:
	cmp rdx, 32
	je .xor
	add [r10], r11
	inc r10
	inc r11
	inc rdx

.xor:
	mov bl, [mat + rax] ; risky
	xor [r8], bl
	inc r8
	inc r9
	jmp .run

.fini:
	pushx r8, r9, r10, r11, rdx
	ret

mod64:
	push r9
.ope:
	cmp r9, 64
	jle .ret
	sub r9, 64
	jmp .ope
.ret:
	mov rax, r9
	pop r9
	ret

;##############################################################################

.data:
	
	woody db "WOODY"
	
	mat dd	0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, \
			0x00000042, 0x00000000, 0x00000000, 0x00000000, \
			0x00000000, 0x00000000, 0x00000000, 0x00000000, \
			0x00000000, 0x00000000, 0x00000042, 0x00000042

	cpy dd	0, 0, 0,0 , \
			0, 0, 0, 0, \
			0, 0, 0, 0, \
			0, 0, 0, 0


