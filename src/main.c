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
	uint32_t	key[8];

	if (ac < 2)
		return ft_error("Usage: ./woody <elf64 executable>\n");

	memset(&elf, 0, sizeof(elf64_t));
	if (elf64_parse(path, &elf) < 0)
		return ft_error("woody: Failed to parse av[1]. Make sure it does existes and is a valid elf64 file.\n");

	if (encrypt_text_section(&elf, key) < 0)
		return ft_error("woody: Failed to encrypt text section.\n");

	if (elf64_add_section(&elf, ".woody", NULL) < 0)
		return ft_error("woody: Failed to add the woody section.\n");

	elf64_cleanup(&elf);
	return (0);
}
