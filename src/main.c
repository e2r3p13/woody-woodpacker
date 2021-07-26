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
	// .reginit
	0x57,  
	0x56, 
	0x52,                    
	0x50,                    
	0x41,  0x51,                 
	0x41,  0x52,                 
	0x41,  0x53,                 
	0x41,  0x54,                 
	0x41,  0x55,                 
	0x41,  0x56,

	// .varinit
	0x41,  0xb9,  0x42,  0x00,  0x00,  0x00,     
	0x4d,  0x89,  0xca,              
	0x41,  0xbb,  0x42,  0x00,  0x00,  0x00,     
	0x41,  0xbc,  0x00,  0x00,  0x00,  0x00,     
	0x41,  0xbd,  0x42,  0x00,  0x00,  0x00,     
	0x41,  0xbe,  0x42,  0x00,  0x00,  0x00,

	// .addrights

	0x41, 0x52,
	0x41, 0x53,
	0x41, 0x54,

	0x4c,  0x89,  0xef,              
	0x4c,  0x89,  0xf6,              
	0xba,  0x07,  0x00,  0x00,  0x00,        
	0xb8,  0x0a,  0x00,  0x00,  0x00,        
	0x0f,  0x05,

	0x41, 0x5c,
	0x41, 0x5b,
	0x41, 0x5a,

	// .decrypt
	0xb3, 0x2a,
	0x4d,  0x39,  0xdc,              
	0x74,  0x0b,                 
	0x41,  0x30,  0x1a,
	0x49,  0xff,  0xc4,              
	0x49,  0xff,  0xc2,              
	0xeb,  0xee,

	// .removerights
	0x4c,  0x89,  0xef,              
	0x4c,  0x89,  0xf6,              
	0xba,  0x05,  0x00,  0x00,  0x00,        
	0xb8,  0x0a,  0x00,  0x00,  0x00,        
	0x0f,  0x05,

	// .print
	0x48,  0xb8,  0x57,  0x6f,  0x6f,  0x64,  0x79,  
	0x0a,  0x00,  0x00,  
	0x50,                    
	0xbf,  0x01,  0x00,  0x00,  0x00,        
	0x48,  0x89,  0xe6,              
	0xba,  0x06,  0x00,  0x00,  0x00,        
	0xb8,  0x01,  0x00,  0x00,  0x00,        
	0x0f,  0x05,                 
	0x58,

	// .regrest
	0x41,  0x5e,                 
	0x41,  0x5d,                 
	0x41,  0x5c,                 
	0x41,  0x5b,                 
	0x41,  0x5a,                 
	0x58,                    
	0x58,                    
	0x5a,                    
	0x5e,                    
	0x5f,

	// .return
	0x41,  0x51,                 
	0xc3                    
};

static void prepare_stub(uint32_t oep, uint32_t tsz, t_key key)
{
	uint32_t	basepageaddr;
	uint32_t	pagelen;

	basepageaddr = oep - (oep % 4096);
	pagelen = tsz + (oep - basepageaddr);
	
	// memcpy(stub + key_off, key, 16);
	memcpy(xor + 18, &oep, sizeof(uint32_t));
	memcpy(xor + 27, &tsz, sizeof(uint32_t));
	memcpy(xor + 39, &basepageaddr, sizeof(uint32_t));
	memcpy(xor + 45, &pagelen, sizeof(uint32_t));

	printf("oep: %x\nsze: %x\npag: %x\nlen: %x\n", oep, tsz, basepageaddr, pagelen);
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
			printf("text size: 0x%x\n", *tsz);
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
	uint32_t	tsz;

	if (ac != 2)
	{
		printf("Usage: %s <t_elf executable>\n", av[0]);
		return (1);
	}
	if ((elf = elf64_read(av[1])) == NULL)
		return (1);
	if (elf64_is_already_packed(elf) || chacha20_keygen(key) < 0 || xor_encrypt(elf, &tsz) < 0) // elf64_encrypt_section(elf, ".text", key) < 0)
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
