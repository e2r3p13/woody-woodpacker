/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfalkau <lfalkau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/17 02:40:05 by lfalkau           #+#    #+#             */
/*   Updated: 2021/07/21 20:54:17 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <elf.h>
#include <stdlib.h>
#include <stdio.h>

#include <woody.h>

int	ft_error(const char *errmsg)
{
	write(2, errmsg, strlen(errmsg));
	return (1);
}

int encrypt_text_section(elf64_t *elf, uint32_t key[8])
{
	if (chacha20_generate_key(key) < 0)
		return (-1);
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(elf64_get_section_name(elf, &elf->sections[i]), ".text") == 0)
			chacha20_encrypt(elf->data, elf->sections[i].sh_offset, elf->sections[i].sh_size, key);
	}
	return (0);
}

int	main(int ac, char **av)
{
	char		*path = av[1];
	elf64_t		elf;
	// uint32_t	key[8];
    char        shellcode[] =
    {
        0x57, 0x56, 0x52, 0x50, 0x48, 0xb8, 0x57, 0x6f, 0x6f, 0x64, 0x79, 0x0a,
        0x00, 0x00, 0x50, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x50, 0xbf, 0x01, 0x00,
        0x00, 0x00, 0x48, 0x89, 0xe6, 0xba, 0x06, 0x00, 0x00, 0x00, 0xb8, 0x01,
        0x00, 0x00, 0x00, 0x0f, 0x05, 0x58, 0x58, 0x5a, 0x5e, 0x5f, 0xc3, 0
    };

	if (ac < 2)
		return ft_error("Usage: ./woody <elf64 executable>\n");

	memset(&elf, 0, sizeof(elf64_t));
	if (elf64_parse(path, &elf) < 0)
		return ft_error("woody: Failed to parse av[1]. Make sure it does existes and is a valid elf64 file.\n");

	// if (encrypt_text_section(&elf, key) < 0)
		// return ft_error("woody: Failed to encrypt text section.\n");

	if (elf64_add_section(&elf, shellcode, 47) < 0)
		return ft_error("woody: Failed to add the woody section.\n");
    
    int fd = open("woody", O_WRONLY | O_CREAT, 00777);
    write(fd, elf.data, elf.size);
    close(fd);

	elf64_cleanup(&elf);
	return (0);
}
