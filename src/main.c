/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:40:55 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 17:52:42 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <stdio.h>
#include <woody.h>
#include <string.h>
#include <chacha20.h>

uint8_t jmp[] =
{
	0x68, 0xff, 0xff, 0xff, 0xff,
	0xc3,
};

uint8_t stub[] =
{
	0x57,										// push   %rdi
	0x56,										// push   %rsi
	0x52,										// push   %rdx
	0x50,										// push   %rax
	0x41, 0x51,									// push   %r9
	0x42, 0x52,									// push   %r10
	0x41, 0xb9, 0x42, 0x00, 0x00, 0x00,			// mov    $0x42,%r9d
    0x41, 0xba, 0x42, 0x00, 0x00, 0x00,			// mov    $0x42,%r10d
    0x4c, 0x89, 0xcf,							// mov    %r9,%rdi
    0x4c, 0x89, 0xd6,							// mov    %r10,%rsi
	0xba, 0x02, 0x00, 0x00, 0x00,				// mov    $0x2,%edx
    0xb8, 0x0a, 0x00, 0x00, 0x00,				// mov    $0xa,%eax
    0x0f, 0x05,									// syscall 
    0x4c, 0x89, 0xcf,							// mov    %r9,%rdi
    0x4c, 0x89, 0xd6,							// mov    %r10,%rsi
    0xba, 0x04, 0x00, 0x00, 0x00,				// mov    $0x4,%edx
    0xb8, 0x0a, 0x00, 0x00, 0x00,				// mov    $0xa,%eax
    0x0f, 0x05,									// syscall 
    0x48, 0xb8, 0x57, 0x6f, 0x6f, 0x64, 0x79,	// movabs $0xa79646f6f57,%rax
    0x0a, 0x00, 0x00,
    0x50,										// push   %rax
    0xbf, 0x01, 0x00, 0x00, 0x00,				// mov    $0x1,%edi
    0x48, 0x89, 0xe6,							// mov    %rsp,%rsi
    0xba, 0x06, 0x00, 0x00, 0x00,				// mov    $0x6,%edx
    0xb8, 0x01, 0x00, 0x00, 0x00,				// mov    $0x1,%eax
    0x0f, 0x05,									// syscall 
    0x58,										// pop    %rax
    0x41, 0x5a,									// pop    %r10
    0x58,										// pop    %rax
    0x58,										// pop    %rax
    0x5a,										// pop    %rdx
    0x5e,										// pop    %rsi
	0x5f,										// pop    %rdi
    0x41, 0x51,									// push   %r9
    0xc3,										// retq	
};

static void prepare_stub(uint32_t oep, uint64_t tsz, t_key key)
{
	size_t	key_off = 0;
	size_t	tsz_off = 0;
	size_t	jmp_off = 1;

	// memcpy(stub + key_off, key, 16);
	memcpy(jmp + jmp_off, &oep, sizeof(uint32_t));
	// memcpy(stub + tsz_off, &tsz, sizeof tsz);
}

int main(int ac, char **av)
{
	t_elf		*elf;
	t_key		key;
	char        out[] = "woody";
	uint64_t	tsz = 0; // TODO

	if (ac != 2)
	{
		printf("Usage: %s <t_elf executable>\n", av[0]);
		return (1);
	}
	if ((elf = elf64_read(av[1])) == NULL)
		return (1);
	if (elf64_is_already_packed(elf) || chacha20_keygen(key) < 0)// || elf64_encrypt_section(elf, ".text", key) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	chacha20_keyprint(key);
	prepare_stub(elf->header.e_entry, tsz, key);
	if (elf64_inject(elf, jmp, sizeof(jmp)) < 0 || elf64_write(elf, out) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	elf64_free(elf);
	return (0);
}
