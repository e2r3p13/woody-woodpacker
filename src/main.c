/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:40:55 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 17:52:42 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <stdio.h>
#include <woody.h>
#include <string.h>
#include <stdlib.h>
#include <chacha20.h>

extern uint8_t	stub[];
extern uint32_t	stub_len;

/*
 *	Will replace stub hardcoded values with computed ones before injection.
 *	Assumes that every hardcoded values are 0x42 and every other bytes are
 *	different.
 *	Also assumes that hardcoded values are always in this order:
 *	oep / text section size / page base address / page offset / chacha key
*/
static void prepare_stub(uint32_t oep, uint32_t txtsecsz, t_key key)
{
	int			offsets[5] = {0};
	uint32_t	basepageaddr = oep - (oep % 4096);
	uint32_t	pageoff = txtsecsz + (oep - basepageaddr);
		
	for (uint32_t i = 0, j = 0; j < stub_len && i < 5; j++)
	{
		if (stub[j] == 0x42)
			offsets[i++] = j;
	}
	if (offsets[4] == 0)
	{
		printf("Error: Missing values to replace in the shellcode\n");
		exit(1);
	}

	memcpy(stub + offsets[0], &oep, sizeof(uint32_t));
	memcpy(stub + offsets[1], &txtsecsz, sizeof(uint32_t));
	memcpy(stub + offsets[2], &basepageaddr, sizeof(uint32_t));
	memcpy(stub + offsets[3], &pageoff, sizeof(uint32_t));
	memcpy(stub + offsets[4] - 4, key, 8 * sizeof(uint32_t));
}

int main(int ac, char **av)
{
	t_elf		*elf;
	t_key		key;
	char        out[] = "woody";
	uint32_t	txtsecsz;
	
	if (ac != 2)
	{
		printf("Usage: %s <t_elf executable>\n", av[0]);
		return (1);
	}
	if ((elf = elf64_read(av[1])) == NULL)
		return (1);
	if (elf64_is_already_packed(elf) || chacha20_keygen(key) < 0 || elf64_encrypt_section(elf, ".text", key, &txtsecsz) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	chacha20_keyprint(key);
	prepare_stub(elf->header.e_entry, txtsecsz, key);
	if (elf64_inject(elf, stub, stub_len) < 0 || elf64_write(elf, out) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	elf64_free(elf);
	return (0);
}
