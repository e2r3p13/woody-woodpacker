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
	0xeb, 0xfe,
	0x68, 0xff, 0xff, 0xff, 0xff,
	0xc3,
};

uint8_t xor[] =
{
	0x57,                   					// push   %rdi
	0x56,                   					// push   %rsi
	0x52,                   					// push   %rdx
	0x50,                   					// push   %rax
	0x41, 0x51,                					// push   %r9
	0x41, 0x52,                					// push   %r0x10,
	0x41, 0x53,                					// push   %r0x11,
	0x41, 0x54,                					// push   %r0x12,

	0x41, 0xb9, 0x42, 0x00, 0x00, 0x00,    		// mov    $0x0x42,,%r0x9d,
	0x4d, 0x89, 0xca,             				// mov    %r9,%r10,
	0x41, 0xbb, 0x42, 0x00, 0x00, 0x00,			// mov    $0x0x42,,%r0x11,d
	0x41, 0xbc, 0x00, 0x00, 0x00, 0x00,			// mov    $0x0,%r0x12,d

	0x4c, 0x89, 0xcf,             				// mov    %r9,%rdi
	0xbe, 0x00, 0x20, 0x00, 0x00,  				// mov    %r0x11,,%rsi
	0xba, 0x07, 0x00, 0x00, 0x00,				// mov    $0x2,%0xed,x
	0xb8, 0x0a, 0x00, 0x00, 0x00,				// mov    $0xa,%0xea,x
	0x0f, 0x05,                					// syscall 

	0x4d, 0x39, 0xdc,             				// cmp    %r11,%r12,
	0x74, 0x0d,							     // je     0x42, <.removerights>
	0xb3, 0x2a,
	0x41, 0x30, 0x1a,
	// 0x41, 0x80, 0x32, 0x2a,
	0x49, 0xff, 0xc2,							// inc    %r10
	0x49, 0xff, 0xc4,							// inc    %r12
	0xeb, 0xee,                					// jmp    0x33, <.0xde,crypt>

	0x4c, 0x89, 0xcf,             				// mov    %r9,%rdi
	0x4c, 0x89, 0xde,             				// mov    %r0x11,,%rsi
	0xba, 0x04, 0x00, 0x00, 0x00,      			// mov    $0x4,%0xed,x
	0xb8, 0x0a, 0x00, 0x00, 0x00,      			// mov    $0xa,%0xea,x
	0x0f, 0x05,                					// syscall 

	0x48, 0xb8, 0x57, 0x6f, 0x6f, 0x64, 0x79,	// mov0xab,s $0x0xa7,0x96,0x46,0xf6,0xf5,7,%rax
	0x0a, 0x00, 0x00, 
	0x50,                   					// push   %rax
	0xbf, 0x01, 0x00, 0x00, 0x00,       		// mov    $0x1,%0xed,i
	0x48, 0x89, 0xe6,             				// mov    %rsp,%rsi
	0xba, 0x06, 0x00, 0x00, 0x00,       		// mov    $0x6,%0xed,x
	0xb8, 0x01, 0x00, 0x00, 0x00,       		// mov    $1%eax
	0x0f, 0x05,                					// syscall 
	0x58,                   					// pop    %rax

	0x41, 0x5c,                					// pop    %r0x12,
	0x41, 0x5b,                					// pop    %r0x11,
	0x41, 0x5a,                					// pop    %r0x10,
	0x58,                   					// pop    %rax
	0x58,                   					// pop    %rax
	0x5a,                   					// pop    %rdx
	0x5e,                   					// pop    %rsi
	0x5f,                   					// pop    %rdi

	0x41, 0x51,                					// push   %r9
	0xc3,                   					// retq
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
	0xba, 0x02, 0x00, 0x00, 					// mov    $0x2,%edx
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

static void prepare_stub(uint32_t oep, uint32_t tsz, t_key key)
{
	size_t	key_off = 0;
	size_t	tsz_off = 23;
	size_t	jmp_off = 14;

	// memcpy(stub + key_off, key, 16);
	oep = oep - (oep % 4096);
	memcpy(xor + jmp_off, &oep, sizeof(uint32_t));
	memcpy(xor + tsz_off, &tsz, sizeof(uint32_t));
}

static int xor_encrypt(t_elf *elf, uint32_t *tsz)
{
	uint8_t	*txt;

	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(elf64_get_section_name(elf, i), ".text") == 0)
		{
			txt = (uint8_t *)elf->scontent[i];
			*tsz = elf->sheaders[i].sh_size;
			for (uint32_t j = 0; j < *tsz; j++)
				txt[j] ^= 0x2a;
			
			return (0);
		}
	}
	return (-1);
}

int main(int ac, char **av)
{
	t_elf		*elf;
	t_key		key;
	char        out[] = "woody";
	uint32_t	tsz = 0; // TODO

	if (ac != 2)
	{
		printf("Usage: %s <t_elf executable>\n", av[0]);
		return (1);
	}
	if ((elf = elf64_read(av[1])) == NULL)
		return (1);
	if (elf64_is_already_packed(elf) || chacha20_keygen(key) < 0)// || xor_encrypt(elf, &tsz) < 0)// elf64_encrypt_section(elf, ".text", key) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	chacha20_keyprint(key);
	prepare_stub(elf->header.e_entry, tsz, key);
	if (elf64_inject(elf, xor, sizeof(xor)) < 0 || elf64_write(elf, out) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	elf64_free(elf);
	return (0);
}
