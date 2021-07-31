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

bits 64

global stub
extern errno

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


;##############################################################################

.data:

	woody db "WOODY", 0xa

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

	call chacha20_run
	
.removerights:
	mov rdi, r10	;
	mov rsi, r11	;
	mov rdx, 5		; PROT_EXEC | PROT_READ 
	mov rax, 10		; sys_mprotec
	syscall

.print:
	mov rdi, 1				; stdout
	lea rsi, [rel woody]	;
	mov rdx, 6				; len
	mov rax, 1				; write
	syscall					;

.fini:
	popx rdi, rsi, rdx, rax, rax, r9, r10, r11

	push r8			; jmp to oep (r8 is a scratch register)
	ret

;##############################################################################

.data:
	
	model dd	0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, \
				0x00000042, 0x00000000, 0x00000000, 0x00000000, \
				0x00000000, 0x00000000, 0x00000000, 0x00000000, \
				0x00000000, 0x00000000, 0x4a000000, 0x00000000
	
	state dd	0, 0, 0, 0, \
				0, 0, 0, 0, \
				0, 0, 0, 0, \
				0, 0, 0, 0

;##############################################################################

chacha20_run:

.init:
	pushx r8, r9, r10, r12, rdi, rbx

	mov r8,	rdi				; iterator through text bytes
	mov r9, 0				; iterator through text length
	mov r10, 0				; counter

.run:
	cmp r9, rsi				; if r9 == length
	je .fini				;
	mov rdi, r9				;
	call mod64				; if r9 % 64 == 0
	cmp rax, 0				;
	jne .xor				;
	
	mov rdi, r10
	call chacha20_block

.xor:
	lea r12, [rel state]	; r11 = stream
	mov bl, [r12 + rax]		; bl = stream[r9 % 64] (rax still holds r9 % 64)
	xor [r8], bl			; *r8 ^= bl
	inc r8					; r8++
	inc r9					; r9++
	jmp .run

.fini:
	popx r8, r9, r10, r12, rdi, rbx
	ret

;##############################################################################

%macro qrop 4
	mov r10d, dword [%2]
	add [%1],r10d
	mov r9d, dword [%1]
	xor [%3], r9d
	rol dword [%3], %4
%endmacro

%macro qr 5
	lea rdi, [%1 + %2 * 4]	;rdi = a
	lea rsi, [%1 + %3 * 4]	;rsi = b
	lea rdx, [%1 + %4 * 4]	;rdx = c
	lea rcx, [%1 + %5 * 4]	;rcx = d
	qrop rdi, rsi, rcx, 16	; a, b, d, 16
	qrop rdx, rcx, rsi, 12	; c, d, b, 12
	qrop rdi, rsi, rcx, 8	; a, b, d, 8
	qrop rdx, rcx, rsi, 7	; c, d, b, 7
%endmacro

chacha20_block:

.init:
	pushx r9, r10, r11, r12, rdi, rsi, rdx, rcx, 

.inc_model_counter:
	lea r10, [rel model]
	inc dword [r10 + 48]

.state_init:
	lea r11, [rel state]
	xor rcx, rcx
	.cpystart:
	cmp rcx, 64
	je .block_init
	xor r12, r12
	mov r12b, [byte r10]
	mov [r11], r12b
	inc r10
	inc r11
	inc rcx
	jmp .cpystart

.block_init:
	mov rcx, 10

.block:

	cmp rcx, 0
	je .state_add

	lea r12, [rel state]

	pushx rdi, rsi, rdx, rcx
	qr r12, 0, 4, 8, 12
	qr r12, 1, 5, 9, 13
	qr r12, 2, 6, 10, 14
	qr r12, 3, 7, 11, 15
	qr r12, 0, 5, 10, 15
	qr r12, 1, 6, 11, 12
	qr r12, 2, 7, 8, 13
	qr r12, 3, 4, 9, 14
	popx rdi, rsi, rdx, rcx

	dec rcx
	jmp .block

.state_add:
	lea r10, [rel model]
	lea r11, [rel state]
	xor rcx, rcx
	.addstart:
	cmp rcx, 64
	je .fini
	mov r12b, [r10]
	add [r11], r12b
	inc r10
	inc r11
	inc rcx
	jmp .addstart

.fini:
	popx r9, r10, r11, r12, rdi, rsi, rdx, rcx
	ret

;##############################################################################

mod64:
.mod:
	mov rax, rdi
	and rax, 63
	ret

