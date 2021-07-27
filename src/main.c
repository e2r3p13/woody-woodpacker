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
#include <chacha20.h>

extern uint8_t	stub[];
extern uint32_t	stub_len;

/*
 *	Will replace stub hardcoded values with computed ones before injection.
 *	Assumes that every hardcoded values are 0x42 and every other bytes are
 *	different.
 *	Also assumes that hardcoded values are always in this order:
 *	oep / text section size / page base address / page offset/ chacha key
*/
static void prepare_stub(uint32_t oep, uint32_t txtsecsz, t_key key)
{
	int			offsets[5];
	uint32_t	basepageaddr = oep - (oep % 4096);
	uint32_t	pageoff = txtsecsz + (oep - basepageaddr);
		
	for (int i = 0, j = 0; j < txtsecsz && i < 5; j++)
	{
		if (stub[j] == 0x42)
			offsets[i++] = j;
	}

	memcpy(stub + offsets[0], &oep, sizeof(uint32_t));
	memcpy(stub + offsets[1], &txtsecsz, sizeof(uint32_t));
	memcpy(stub + offsets[2], &basepageaddr, sizeof(uint32_t));
	memcpy(stub + offsets[3], &pageoff, sizeof(uint32_t));
}

static int xor_encrypt(t_elf *elf, uint32_t *tsz)
{
	uint8_t	*txt;

	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (strcmp(elf64_get_section_name(elf, i), ".text") == 0)
		{
			txt = (uint8_t *)elf->scontent[i];
			*tsz = elf->sheaders[i].sh_size;
			printf("text size: 0x%x\n", *tsz);
			for (uint32_t j = 0; j < *tsz; j++)
				txt[j] ^= 0x2a;
			
			return (0);
		}
	}
	return (-1);
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
