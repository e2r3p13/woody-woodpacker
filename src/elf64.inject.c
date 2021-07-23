/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.inject.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:38:27 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 01:15:24 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>
#include <string.h>
#include <stdlib.h>
#include <elf.h>


static Elf64_Phdr *get_ptnote(t_elf *elf)
{
	for (size_t i = 0; i < elf->header.e_phnum; i++)
	{
		if (elf->pheaders[i].p_type == PT_NOTE)
			return (elf->pheaders + i);
	}
	return (NULL);
}

static Elf64_Shdr *get_note_section(t_elf *elf, size_t *notendx)
{
	for (size_t i = 0; i < elf->header.e_shnum; i++)
	{
		if (elf->sheaders[i].sh_type == SHT_NOTE)
		{
			*notendx = i;
			return (elf->sheaders + i);
		}
	}
	return (NULL);
}

int elf64_inject(t_elf *elf, uint8_t *loader, size_t lsize)
{
	Elf64_Phdr	*note_segment;
	Elf64_Shdr	*note_section;
	size_t		note_secndx;
	char		*note_secname;
	uint8_t		*new_note_content;

	size_t		loader_offset;
	size_t		loader_vaddr;

	// search for a note segment and a note section
	note_segment = get_ptnote(elf);
	note_section = get_note_section(elf, &note_secndx);
	if (note_segment == NULL || note_section == NULL)
		return (-1);

	// Replace the note section content with our payload
	new_note_content = malloc(sizeof(uint8_t) * lsize);
	if (new_note_content == NULL)
		return (-1);
	memcpy(new_note_content, loader, lsize);
	free(elf->scontent[note_secndx]);
	elf->scontent[note_secndx] = new_note_content;

	// Compute the offset and the addr of our injected code
	loader_offset = elf->header.e_shoff + (elf->header.e_shnum * elf->header.e_shentsize);
	loader_offset += 4096 - (loader_offset % 4096);
	loader_vaddr = 0xc000000;
	loader_vaddr += 4096 - (loader_vaddr % 4096);

	// Corrupt the note section to inject our payload
	note_section->sh_type = SHT_PROGBITS;
	note_section->sh_flags = SHF_EXECINSTR;
	note_section->sh_offset = loader_offset;
	note_section->sh_addr = loader_vaddr;
	note_section->sh_size = lsize;
	note_section->sh_addralign = 16;

	// Rename the corrupted section to detect previous packed binary and avoid to repack it
	note_secname = elf64_get_section_name(elf, note_secndx);
	strcpy(note_secname, ".woody");

	// Corrupt the note segment to make it loadable / readable / executable and point to our corrupted note section
	note_segment->p_type = PT_LOAD;
	note_segment->p_flags = PF_X | PF_R;
	note_segment->p_offset = loader_offset;
	note_segment->p_align = 16;
	note_segment->p_vaddr = loader_vaddr;
	note_segment->p_filesz = lsize;
	note_segment->p_memsz = lsize;

	// Change the program entrypoint to our loader
	elf->header.e_entry = loader_vaddr;

	return (0);
}
