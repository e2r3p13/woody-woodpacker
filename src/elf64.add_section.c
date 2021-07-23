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

static size_t find_target_section_index(Elf64 *elf)
{
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (elf->sheaders[i].sh_type == SHT_NOBITS)
			return (i);
	}
	return (0);
}

static Elf64_Phdr *get_last_ptload(Elf64 *elf)
{
	size_t last = 0;
	
	for (size_t i = 0; i < elf->header.e_phnum; i++)
	{
		if (elf->pheaders[i].p_type == PT_LOAD)
			last = i;
	}
	return (last > 0 ? elf->pheaders + last : NULL);
}

static Elf64_Phdr *get_ptnote(Elf64 *elf)
{
	for (size_t i = 0; i < elf->header.e_phnum; i++)
	{
		if (elf->pheaders[i].p_type == PT_NOTE)
			return (elf->pheaders + i);
	}
	return (NULL);
}

int elf64_inject_loader(Elf64 *elf, uint8_t *loader, size_t lsize)
{
	size_t		cndx;
	Elf64_Shdr	*corrupted;
	Elf64_Phdr	*last_ptload;
	Elf64_Phdr	*ptnote;

	cndx = find_target_section_index(elf);
	if (cndx == 0)
		return (-1);
	corrupted = elf->sheaders + cndx;

	if (corrupted->sh_size > lsize)
	{
		memset(elf->scontent[cndx], 0, corrupted->sh_size);
		memcpy(elf->scontent[cndx], loader, lsize);
	}
	else
	{
		uint8_t *new_content = malloc(sizeof(uint8_t) * lsize);
		if (new_content == NULL)
			return (-1);
		memcpy(new_content, loader, lsize);
		free(elf->scontent[cndx]);
		elf->scontent[cndx] = new_content;
		corrupted->sh_size = lsize;
	}

	last_ptload = get_last_ptload(elf);
	if (last_ptload == NULL)
		return (-1);

	ptnote = get_ptnote(elf);
	if (ptnote == NULL)
		return (-1);

	// Make the last loadable segment longer enough to contain the corrupted section
	//last_ptload->p_filesz = (corrupted->sh_addr - last_ptload->p_vaddr) + corrupted->sh_size;
	//last_ptload->p_memsz = last_ptload->p_filesz;
	//last_ptload->p_flags = PF_X | PF_R;
	
	// Modify the ptnote segment to point to the corrupted section
	ptnote->p_type = PT_LOAD;
	ptnote->p_flags = PF_X | PF_R;
	ptnote->p_vaddr += corrupted->sh_addr;
	//ptnote->p_paddr += corrupted->sh_addr;
	//ptnote->p_filesz = corrupted->sh_size;
	//ptnote->p_memsz = corrupted->sh_size;

	//elf->header.e_entry = ptnote->p_vaddr;
	return (0);	
}
