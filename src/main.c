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

int main(int ac, char **av)
{
	Elf64		*elf;
	Cha20Key	key;

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
	if (chacha20_keygen(key) < 0)
	{
		printf("%s: Failed to generate a chacha20 key\n", av[0]);
		elf64_free(elf);
		return (1);
	}
	elf64_print(elf, 0b11100);
	elf64_encrypt_section(elf, ".text", key);
	elf64_print(elf, 0b11100);
	elf64_free(elf);
	return (0);
}
