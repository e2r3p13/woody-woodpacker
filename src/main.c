/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:40:55 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 01:44:53 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <stdio.h>
#include <woody.h>
#include <chacha20.h>

uint8_t stub[] =
{
	0xeb, 0xfe
};

int main(int ac, char **av)
{
	t_elf		*elf;
	Cha20Key	key;
	char        out[] = "woody";

	if (ac != 2)
	{
		printf("Usage: %s <t_elf executable>\n", av[0]);
		return (1);
	}

	if ((elf = elf64_read(av[1])) == NULL)
		return (1);

	if (chacha20_keygen(key) < 0 ||
		elf64_encrypt_section(elf, ".text", key) < 0 ||
		elf64_inject(elf, stub, sizeof(stub)) < 0 ||
		elf64_write(elf, out) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	
	elf64_free(elf);
	return (0);
}
