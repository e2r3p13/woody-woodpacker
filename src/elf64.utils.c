/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 21:34:31 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 22:16:21 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <string.h>
#include <woody.h>
#include <chacha20.h>

char *elf64_get_section_name(Elf64 *elf, size_t index)
{
	size_t	secname_index;

	secname_index = elf->header.e_shstrndx;
	return ((char *)(elf->scontent[secname_index] + elf->sheaders[index].sh_name));
}

void	elf64_encrypt_section(Elf64 *elf, const char *sname, Cha20Key key)
{
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(sname, elf64_get_section_name(elf, i)) == 0)
		{
			chacha20_encrypt(elf->scontent[i], 0, elf->sheaders[i].sh_size, key);
			break ;
		}
	}
}
