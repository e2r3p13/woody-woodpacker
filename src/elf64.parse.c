/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/20 17:29:21 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/21 17:47:24 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <stdlib.h>
#include <woody.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

char	*elf64_get_section_name(elf64_t *elf, Elf64_Shdr *section)
{
	Elf64_Shdr	*secnames;
	size_t		pos;

	secnames = &(elf->sections[elf->header.e_shstrndx]);
	pos = secnames->sh_offset + section->sh_name;
	return (elf->data + pos);
}

void	elf64_cleanup(elf64_t *elf)
{
	if (elf != NULL)
	{
		if (elf->data != NULL)
			free(elf->data);
		if (elf->segments != NULL)
			free(elf->segments);
		if (elf->sections != NULL)
			free(elf->sections);
	}
}

static int	elf64_parse_header(elf64_t *elf)
{
	Elf64_Ehdr	*header = &elf->header;
	char		*data = elf->data;
	size_t		size = elf->size;
	char		expected_ident[] = {0x7f, 'E', 'L', 'F', 2};

	if (size < 0x40)
		return (-1);
	memcpy(header->e_ident, data, EI_NIDENT);
	if (memcmp(header->e_ident, expected_ident, sizeof expected_ident) != 0)
		return (-1);
	header->e_type = *(Elf64_Half *)(data + 0x10);
	header->e_machine = *(Elf64_Half *)(data + 0x12);
	header->e_version = *(Elf64_Word *)(data + 0x14);
	header->e_entry = *(Elf64_Addr *)(data + 0x18);
	header->e_phoff = *(Elf64_Off *)(data + 0x20);
	header->e_shoff = *(Elf64_Off *)(data + 0x28);
	header->e_flags = *(Elf64_Word *)(data + 0x30);
	header->e_ehsize = *(Elf64_Half *)(data + 0x34);
	header->e_phentsize = *(Elf64_Half *)(data + 0x36);
	header->e_phnum = *(Elf64_Half *)(data + 0x38);
	header->e_shentsize = *(Elf64_Half *)(data + 0x3a);
	header->e_shnum = *(Elf64_Half *)(data + 0x3c);
	header->e_shstrndx = *(Elf64_Half *)(data + 0x3e);
	return (0);
}

static void	elf64_parse_segment(Elf64_Phdr *header, const char *data, size_t offset)
{
	header->p_type = *(Elf64_Word *)(data + offset + 0x0);
	header->p_flags = *(Elf64_Word *)(data + offset + 0x04);
	header->p_offset = *(Elf64_Off *)(data + offset + 0x08);
	header->p_vaddr = *(Elf64_Addr *)(data + offset + 0x10);
	header->p_paddr = *(Elf64_Addr *)(data + offset + 0x18);
	header->p_filesz = *(Elf64_Xword *)(data + offset + 0x20);
	header->p_memsz = *(Elf64_Xword *)(data + offset + 0x28);
	header->p_align = *(Elf64_Xword *)(data + offset + 0x30);
}

static int	elf64_parse_segments(elf64_t *elf)
{
	Elf64_Ehdr	*header = &elf->header;

	if (elf->size < header->e_phoff + header->e_phentsize * header->e_phnum)
	{
		return (-1);
	}
	elf->segments = malloc(sizeof(Elf64_Phdr) * header->e_phnum);
	if (elf->segments == NULL)
	{
		return (-1);
	}
	for (size_t i = 0; i < header->e_phnum; i++)
	{
		elf64_parse_segment(&elf->segments[i], elf->data, header->e_phoff + header->e_phentsize * i);
	}
	return (0);
}

static void	elf64_parse_section(Elf64_Shdr *header, const char *data, size_t offset)
{
	header->sh_name = *(Elf64_Word *)(data + offset + 0x00);
	header->sh_type = *(Elf64_Word *)(data + offset + 0x04);
	header->sh_flags = *(Elf64_Xword *)(data + offset + 0x08);
	header->sh_addr = *(Elf64_Addr *)(data + offset + 0x10);
	header->sh_offset = *(Elf64_Off *)(data + offset + 0x18);
	header->sh_size = *(Elf64_Xword *)(data + offset + 0x20);
	header->sh_link = *(Elf64_Word *)(data + offset + 0x28);
	header->sh_info = *(Elf64_Word *)(data + offset + 0x2c);
	header->sh_addralign = *(Elf64_Xword *)(data + offset + 0x30);
	header->sh_entsize = *(Elf64_Xword *)(data + offset + 0x38);
}

static int	elf64_parse_sections(elf64_t *elf)
{
	Elf64_Ehdr	*header = &elf->header;

	if (elf->size < header->e_shoff + header->e_shentsize * header->e_shnum)
	{
		return (-1);
	}
	elf->sections = malloc(sizeof(Elf64_Shdr) * header->e_shnum);
	if (elf->sections == NULL)
	{
		return (-1);
	}
	for (size_t i = 0; i < header->e_shnum; i++)
	{
		elf64_parse_section(&elf->sections[i], elf->data, header->e_shoff + header->e_shentsize * i);
	}
	return (0);
}

int	elf64_parse(const char *path, elf64_t *elf)
{
	int			fd;
	struct stat	s;

	// Read given file
	fd = open(path, O_RDWR);
	if (fd < 0 || fstat(fd, &s) < 0)
		return (-1);
	elf->size = s.st_size;
	elf->data = malloc(sizeof(char) * elf->size);
	if (elf->data == NULL || read(fd, elf->data, elf->size) < 0)
	{
		close(fd);
		elf64_cleanup(elf);
		return (-1);
	}
	close(fd);

	// Parse file
	if (elf64_parse_header(elf) < 0 || elf64_parse_segments(elf) < 0 || elf64_parse_sections(elf) < 0)
	{
		elf64_cleanup(elf);
		return (-1);
	}
	return (0);
}
