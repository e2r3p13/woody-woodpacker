/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:40:55 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 22:26:14 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <stdio.h>
#include <woody.h>
#include <chacha20.h>

uint8_t shellcode[] =
{
	0x57, 0x56, 0x52, 0x50, 0x48, 0xb8, 0x57, 0x6f, 0x6f, 0x64, 0x79, 0x0a,
	0x00, 0x00, 0x50, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x50, 0xbf, 0x01, 0x00,
	0x00, 0x00, 0x48, 0x89, 0xe6, 0xba, 0x06, 0x00, 0x00, 0x00, 0xb8, 0x01,
	0x00, 0x00, 0x00, 0x0f, 0x05, 0x58, 0x58, 0x5a, 0x5e, 0x5f, 0xc3, 0
};

uint8_t infinite[] =
{
	0xeb, 0xfe
};

int main(int ac, char **av)
{
	Elf64		*elf;
	Cha20Key	key;
	char        outpath[] = "woody";
	
	// Check for arguments
	if (ac != 2)
	{
		printf("Usage: woody_woodpacker <elf64 executable>\n");
		return (1);
	}
	
	// Read and parse the given file into an Elf64 struct
	elf = elf64_read(av[1]);
	if (elf == NULL)
	{
		printf("%s: Error, please make sure '%s' exists and is a valid elf64 file\n", av[0], av[1]);
		return (1);
	}
	
	// Generate a key and encrypt the text section
	if (chacha20_keygen(key) < 0)
	{
		printf("%s: Failed to generate a chacha20 key\n", av[0]);
		elf64_free(elf);
		return (1);
	}
	// elf64_encrypt_section(elf, ".text", key);
	
	elf64_print(elf, 0b00011);

	// Try to find a section to corrupt and inject our shellcode inside
	// Also extend the last segment to make sure the section is loadable
	if (elf64_inject_loader_after_sectable(elf, infinite, sizeof infinite) < 0)
	{
		printf("%s: Failed to inject the loader\n", av[1]);
		elf64_free(elf);
		return (1);
	}

	elf64_print(elf, 0b11);
	
	// Output a new elf file from the Elf64 structure
	if (elf64_write(elf, outpath) < 0)
	{
		printf("%s: Warning, some write operation has failed, the file may not be recognized\n", av[1]);
		elf64_free(elf);
		return (1);
	}
	elf64_free(elf);
	return (0);
}
