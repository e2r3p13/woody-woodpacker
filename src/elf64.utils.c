/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 21:34:31 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 01:50:52 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <woody.h>
#include <chacha20.h>

char *elf64_get_section_name(t_elf *elf, size_t index)
{
	size_t	secname_index;

	secname_index = elf->header.e_shstrndx;
	return ((char *)(elf->scontent[secname_index] + elf->sheaders[index].sh_name));
}

int elf64_encrypt_section(t_elf *elf, const char *sname, Cha20Key key)
{
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(sname, elf64_get_section_name(elf, i)) == 0)
		{
			chacha20_run(elf->scontent[i], 0, elf->sheaders[i].sh_size, key);
			return (0);
		}
	}
	printf("Error: section not found: '%s'\n", sname);
	return (1);
}

long get_file_size(int fd)
{
	long	curr;
	long	size;

	curr = lseek(fd, 0, SEEK_CUR);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, curr, SEEK_SET);

	return (size);
}
