; **************************************************************************** ;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    loader.s                                           :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2021/07/21 19:41:02 by bccyv             #+#    #+#              ;
;    Updated: 2021/07/21 20:38:35 by bccyv            ###   ########.fr        ;
;                                                                              ;
; **************************************************************************** ;

; save registers

push rdi
push rsi
push rdx
push rax

; add write rights to text section

mov rdi, 0x42	; addr of text section
mov rsi, 0x42	; size of text section
mov rdx, 2		; PROT_WRITE
mov rax, 10 	; sys_mprotect
syscall

; decrypt text section

; remove write and add execute rights to text section

mov rdi, 0x42	; addr of text section
mov rsi, 0x42	; size of text section
mov rdx, 4		; PROT_EXEC
mov rax, 10		; sys_mprotect
syscall

; restore registers

pop rax
pop rdx
pop rsi
pop rdi

; jump to OEP

push 0x42		; address of OEP
ret
