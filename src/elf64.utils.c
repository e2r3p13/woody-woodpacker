/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 21:34:31 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 21:35:01 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <woody.h>

char *elf64_get_section_name(Elf64 *elf, Elf64_Shdr *section)
{
	size_t	secname_index;

	secname_index = elf->header.e_shstrndx;
	return ((char *)(elf->scontent[secname_index] + section->sh_name));
}
