/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 21:34:31 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 04:15:39 by bccyv            ###   ########.fr       */
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
	size_t	secndx;

	secndx = elf->header.e_shstrndx;
	return ((char *)(elf->scontent[secndx] + elf->sheaders[index].sh_name));
}

int elf64_encrypt_section(t_elf *elf, const char *sname, t_key key, uint32_t *txtsecsz)
{
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(sname, elf64_get_section_name(elf, i)) == 0)
		{
			*txtsecsz = elf->sheaders[i].sh_size;
			chacha20_run(elf->scontent[i], *txtsecsz, key);
			return (0);
		}
	}
	printf("Error: section not found: '%s'\n", sname);
	return (1);
}

static bool elf64_is_clang_compiled(t_elf *elf)
{
	uint8_t		*secaddr;
	size_t		secsize;
	
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(elf64_get_section_name(elf, i), ".comment") == 0)
		{
			secaddr = elf->scontent[i];
			secsize = elf->sheaders[i].sh_size;
			if (memmem((const void *)secaddr, secsize, "clang", 5) != 0)
				return (true);
			break ;
		}
	}
	printf("woody_woodpacker: given binaries must be compiled with 'clang -m64'\n");
	return (false);
}

static bool elf64_is_already_packed(t_elf *elf)
{
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(elf64_get_section_name(elf, i), ".woody") == 0)
		{
			printf("woody_woodpacker: binary already packed\n");
			return (1);
		}
	}
	return (0);
}

bool elf64_is_packable(t_elf *elf)
{
	return (elf64_is_clang_compiled(elf) && !elf64_is_already_packed(elf));
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
