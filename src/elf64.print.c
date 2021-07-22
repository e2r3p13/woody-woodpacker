/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:16:21 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 21:33:28 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <woody.h>
#include <stdio.h>

static void print_header(Elf64_Ehdr *hdr, int padding)
{
	printf("%*se_ident: %s\n", padding, "", hdr->e_ident);
	printf("%*se_type: 0x%x\n", padding, "", hdr->e_type);
	printf("%*se_machine: 0x%x\n", padding, "", hdr->e_machine);
	printf("%*se_version: 0x%x\n", padding, "", hdr->e_version);
	printf("%*se_entry: 0x%llx\n", padding, "", hdr->e_entry);
	printf("%*se_phoff: %lld\n", padding, "", hdr->e_phoff);
	printf("%*se_shoff: %lld\n", padding, "", hdr->e_shoff);
	printf("%*se_flags: 0x%x\n", padding, "", hdr->e_flags);
	printf("%*se_ehsize: %d\n", padding, "", hdr->e_ehsize);
	printf("%*se_phentsize: %d\n", padding, "", hdr->e_phentsize);
	printf("%*se_phnum: %d\n", padding, "", hdr->e_phnum);
	printf("%*se_shentsize: %d\n", padding, "", hdr->e_shentsize);
	printf("%*se_shnum: %d\n", padding, "", hdr->e_shnum);
	printf("%*se_shstrndx: %d\n", padding, "", hdr->e_shstrndx);
}

static void print_pheader(Elf64_Phdr *phdr, int padding)
{
	printf("%*sp_type 0x %x\n", padding, "", phdr->p_type);
	printf("%*sp_flags 0x%x\n", padding, "", phdr->p_flags);
	printf("%*sp_offset %lld\n", padding, "", phdr->p_offset);
	printf("%*sp_vaddr 0x%llx\n", padding, "", phdr->p_vaddr);
	printf("%*sp_paddr 0x%llx\n", padding, "", phdr->p_paddr);
	printf("%*sp_filesz %lld\n", padding, "", phdr->p_filesz);
	printf("%*sp_memsz %lld\n", padding, "", phdr->p_memsz);
	printf("%*sp_align %lld\n", padding, "", phdr->p_align);
}

static void print_sname(char *sname)
{
	printf("%s\n", sname);
}

static void print_sheader(Elf64_Shdr *shdr, int padding)
{
	printf("%*ssh_name: %d\n", padding, "", shdr->sh_name);
	printf("%*ssh_type: 0x%x\n", padding, "", shdr->sh_type);
	printf("%*ssh_flags: 0x%llx\n", padding, "", shdr->sh_flags);
	printf("%*ssh_addr: 0x%llx\n", padding, "", shdr->sh_addr);
	printf("%*ssh_offset: %lld\n", padding, "", shdr->sh_offset);
	printf("%*ssh_size: %lld\n", padding, "", shdr->sh_size);
	printf("%*ssh_link: 0x%x\n", padding, "", shdr->sh_link);
	printf("%*ssh_info: 0x%x\n", padding, "", shdr->sh_info);
	printf("%*ssh_addralign: 0x%llx\n", padding, "", shdr->sh_addralign);
	printf("%*ssh_entsize: %lld\n", padding, "", shdr->sh_entsize);
}

static void print_scontent(uint8_t *scontent, size_t size, int padding)
{
	int	bytes_per_line = 25;

	if (size > 0)
	{
		for (size_t i = 0; i < size - 1; i++)
		{
			if (i % bytes_per_line == 0)
				printf("\n%*s", padding, "");
			printf("%.2x ", scontent[i]);
		}
		printf("%x\n", scontent[size - 1]);
	}
}

void elf64_print(Elf64 *elf, int flags)
{
	int	padding = 4;

	if (flags & 0b00001)
	{
		printf("-- HEADER --\n\n");
		print_header(&elf->header, 0);
	}
	if (flags & 0b00010)
	{
		printf("\n-- SEGMENTS --\n");
		for (size_t i = 0; i < elf->header.e_phnum; i++)
		{
			printf("\n");
			print_pheader(elf->pheaders + i, 0);
		}
	}
	if (flags & 0b00100)
	{
		printf("\n-- SECTIONS --\n");
		for (size_t i = 0; i < elf->header.e_shnum; i++)
		{
			printf("\n");
			print_sname(elf64_get_section_name(elf, elf->sheaders + i));
			if (flags & 0b01000)
				print_sheader(elf->sheaders + i, padding);
			if (flags & 0b10000)
				print_scontent(elf->scontent[i], elf->sheaders[i].sh_size, padding);
		}
	}
}
