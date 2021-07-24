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

; save registers

push rdi
push rsi
push rdx
push rax
push r9
push r10

mov r9, 0x42	; store oep (replaced when packed)
mov r10, 0x42	; store text length

; add write rights to text section

mov rdi, r9		; addr of text section
mov rsi, r10	; size of text section
mov rdx, 2		; PROT_WRITE
mov rax, 10 	; sys_mprotect
syscall

; decrypt text section



; remove write and add execute rights to text section

mov rdi, r9		; addr of text section
mov rsi, r10	; size of text section
mov rdx, 4		; PROT_EXEC
mov rax, 10		; sys_mprotect
syscall

; print "Woody\n"

mov rax, 0xa79646f6f57
push rax

mov rdi, 1		; STDOUT
mov rsi, rsp	; addr of "WOODY\n"
mov rdx, 6		; size of "WOODY\n"
mov rax, 1		; write
syscall

pop rax

; restore registers

pop r10
pop rax			; scratch register, to keep r9 to return
pop rax
pop rdx
pop rsi
pop rdi

; jump to OEP

push r9			; address of OEP
ret
