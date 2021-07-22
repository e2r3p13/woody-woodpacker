/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:16:21 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 16:07:20 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <woody.h>
#include <stdio.h>

static void print_header(Elf64_Ehdr *hdr)
{
	printf("e_ident: %s\n", hdr->e_ident);
	printf("e_type: 0x%x\n", hdr->e_type);
	printf("e_machine: 0x%x\n", hdr->e_machine);
	printf("e_version: 0x%x\n", hdr->e_version);
	printf("e_entry: 0x%llx\n", hdr->e_entry);
	printf("e_phoff: %lld\n", hdr->e_phoff);
	printf("e_shoff: %lld\n", hdr->e_shoff);
	printf("e_flags: 0x%x\n", hdr->e_flags);
	printf("e_ehsize: %d\n", hdr->e_ehsize);
	printf("e_phentsize: %d\n", hdr->e_phentsize);
	printf("e_phnum: %d\n", hdr->e_phnum);
	printf("e_shentsize: %d\n", hdr->e_shentsize);
	printf("e_shnum: %d\n", hdr->e_shnum);
	printf("e_shstrndx: %d\n", hdr->e_shstrndx);
}

static void print_pheader(Elf64_Phdr *phdr)
{
	printf("p_type 0x %x\n", phdr->p_type);
	printf("p_flags 0x%x\n", phdr->p_flags);
	printf("p_offset %lld\n", phdr->p_offset);
	printf("p_vaddr 0x%llx\n", phdr->p_vaddr);
	printf("p_paddr 0x%llx\n", phdr->p_paddr);
	printf("p_filesz %lld\n", phdr->p_filesz);
	printf("p_memsz %lld\n", phdr->p_memsz);
	printf("p_align %lld\n", phdr->p_align);
}

static void print_sname(char *sname)
{
	printf("%s\n", sname);
}

static void print_sheader(Elf64_Shdr *shdr)
{
	printf("sh_name: %d\n", shdr->sh_name);
	printf("sh_type: 0x%x\n", shdr->sh_type);
	printf("sh_flags: 0x%llx\n", shdr->sh_flags);
	printf("sh_addr: 0x%llx\n", shdr->sh_addr);
	printf("sh_offset: %lld\n", shdr->sh_offset);
	printf("sh_size: %lld\n", shdr->sh_size);
	printf("sh_link: 0x%x\n", shdr->sh_link);
	printf("sh_info: 0x%x\n", shdr->sh_info);
	printf("sh_addralign: 0x%llx\n", shdr->sh_addralign);
	printf("sh_entsize: %lld\n", shdr->sh_entsize);
}

static void print_scontent(char *scontent)
{
	printf("[...]\n");
}

void elf64_print(Elf64 *elf, int flags)
{
	if (flags & 0b00001)
	{
		printf("-- HEADER --\n");
		print_header(&elf->header);
	}
	if (flags & 0b00010)
	{
		printf("-- SEGMENTS --\n");
		for (size_t i = 0; i < elf->header.e_phnum; i++)
			print_pheader(elf->pheaders + i);
	}
	if (flags & 0b00100)
	{
		printf("-- SECTIONS --\n");
		for (size_t i = 0; i < elf->header.e_shnum; i++)
		{
			print_sname(elf64_get_section_name(elf, elf->sheaders + i));
			if (flags & 0b01000)
				print_sheader(elf->sheaders + i);
			if (flags & 0b10000)
				print_scontent(elf->scontent[i]);
		}
	}
}
