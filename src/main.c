/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:40:55 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 21:37:09 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <woody.h>
#include <stdio.h>

// char shellcode[] =
// {
// 	0x57, 0x56, 0x52, 0x50, 0x48, 0xb8, 0x57, 0x6f, 0x6f, 0x64, 0x79, 0x0a,
// 	0x00, 0x00, 0x50, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x50, 0xbf, 0x01, 0x00,
// 	0x00, 0x00, 0x48, 0x89, 0xe6, 0xba, 0x06, 0x00, 0x00, 0x00, 0xb8, 0x01,
// 	0x00, 0x00, 0x00, 0x0f, 0x05, 0x58, 0x58, 0x5a, 0x5e, 0x5f, 0xc3, 0
// };

int main(int ac, char **av)
{
	Elf64	*elf;

	if (ac != 2)
	{
		printf("Usage: woody_woodpacker <elf64 executable>\n");
		return (1);
	}
	elf = elf64_read(av[1]);
	if (elf == NULL)
	{
		printf("%s: Error, please make sure '%s' exists and is a valid elf64 file\n", av[0], av[1]);
		return (1);
	}
	elf64_print(elf, 0b11111);
	elf64_free(elf);
	return (0);
}
