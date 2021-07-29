/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.inject.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:38:27 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 17:16:55 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>
#include <string.h>
#include <stdlib.h>
#include <elf.h>


static Elf64_Phdr *get_ptnote_sheader(t_elf *elf)
{
	for (size_t i = 0; i < elf->header.e_phnum; i++)
	{
		if (elf->pheaders[i].p_type == PT_NOTE)
			return (elf->pheaders + i);
	}
	return (NULL);
}

static size_t get_note_shindex(t_elf *elf)
{
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (elf->sheaders[i].sh_type == SHT_NOTE)
			return (i);
	}
	return (0);
}

/*
 *	We update the name of the corrupted section to be able to check if the
 *	binary is packed. It's not very discreet and not mandatory but this
 *	project has educational purpose so be it.
*/
static void corrupt_sheader(t_elf *elf, size_t i, size_t soff, size_t ssize, size_t saddr)
{
	Elf64_Shdr	*shdr = elf->sheaders + i;
	char		*secname;

	secname = elf64_get_section_name(elf, i);
	strcpy(secname, ".woody");

	shdr->sh_type = SHT_PROGBITS;
	shdr->sh_flags = SHF_EXECINSTR;
	shdr->sh_offset = soff;
	shdr->sh_addr = saddr;
	shdr->sh_size = ssize;
	shdr->sh_addralign = 16;
}

static int corrupt_scontent(t_elf *elf, size_t secndx, uint8_t *stub, size_t ssize)
{
	uint8_t	*new_scontent;

	if ((new_scontent = malloc(sizeof(uint8_t) * ssize)) == NULL)
		return (-1);
	memcpy(new_scontent, stub, ssize);
	free(elf->scontent[secndx]);
	elf->scontent[secndx] = new_scontent;
	return (0);
}

static void corrupt_pheader(Elf64_Phdr *phdr, size_t soff, size_t ssize, size_t saddr)
{
	phdr->p_type = PT_LOAD;
	phdr->p_flags = PF_X | PF_R | PF_W;
	phdr->p_offset = soff;
	phdr->p_align = 16;
	phdr->p_vaddr = saddr;
	phdr->p_filesz = ssize;
	phdr->p_memsz = ssize;
}

/*
 *	When injecting the stub, keep in mind that stub_offset % PAGE_SIZE must
 *	be equal to the stub_virtual_addess % PAGE_SIZE
*/
int elf64_inject(t_elf *elf, uint8_t *stub, size_t stub_size)
{
	Elf64_Phdr	*note_pheader;
	size_t		note_shindex;
	size_t		stub_offset;
	size_t		stub_vaddr;

	if ((note_pheader = get_ptnote_sheader(elf)) == NULL)
		return (-1);
	if ((note_shindex = get_note_shindex(elf)) == 0)
		return (-1);
	if (corrupt_scontent(elf, note_shindex, stub, stub_size) < 0)
		return (-1);

	stub_offset = elf->header.e_shoff + (elf->header.e_shnum * elf->header.e_shentsize);
	stub_offset += 4096 - (stub_offset % 4096);
	stub_vaddr = 0xc000000;
	stub_vaddr += 4096 - (stub_vaddr % 4096);

	corrupt_sheader(elf, note_shindex, stub_offset, stub_size, stub_vaddr);
	corrupt_pheader(note_pheader, stub_offset, stub_size, stub_vaddr);

	elf->header.e_entry = stub_vaddr;
	return (0);
}
