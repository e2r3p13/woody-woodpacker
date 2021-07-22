/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.add_section.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:38:27 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 22:30:38 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>
#include <string.h>
#include <stdlib.h>
#include <elf.h>

// char shellcode[] =
// {
// 	0x57, 0x56, 0x52, 0x50, 0x48, 0xb8, 0x57, 0x6f, 0x6f, 0x64, 0x79, 0x0a,
// 	0x00, 0x00, 0x50, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x50, 0xbf, 0x01, 0x00,
// 	0x00, 0x00, 0x48, 0x89, 0xe6, 0xba, 0x06, 0x00, 0x00, 0x00, 0xb8, 0x01,
// 	0x00, 0x00, 0x00, 0x0f, 0x05, 0x58, 0x58, 0x5a, 0x5e, 0x5f, 0xc3, 0
// };

/*
	TODO:
	- Realloc sheaders to insert new section header
	- Realloc name section content to add the new section name
	- Realloc scontent to insert new section content
	- Fill the new section information
	- Update the section table offset in main header
	- Find a way to make the last loadable segment also load the last section
*/
int elf64_add_section(Elf64 *elf, char *sname, uint8_t *scontent, size_t ssize)
{
	Elf64_Shdr	new_section;


	return (0);
}
