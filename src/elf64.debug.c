/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/20 18:17:17 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/21 17:21:08 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <woody.h>
#include <elf.h>

void	elf64_print(elf64_t *elf, int flags)
{
	if (flags & 0b1)
	{
		printf("ELF header:\n");
		printf("\te_ident: %s\n", elf->header.e_ident);
		printf("\te_type: 0x%x\n", elf->header.e_type);
		printf("\te_machine: 0x%x\n", elf->header.e_machine);
		printf("\te_version: 0x%x\n", elf->header.e_version);
		printf("\te_entry: 0x%llx\n", elf->header.e_entry);
		printf("\te_phoff: %lld\n", elf->header.e_phoff);
		printf("\te_shoff: %lld\n", elf->header.e_shoff);
		printf("\te_flags: 0x%x\n", elf->header.e_flags);
		printf("\te_ehsize: %d\n", elf->header.e_ehsize);
		printf("\te_phentsize: %d\n", elf->header.e_phentsize);
		printf("\te_phnum: %d\n", elf->header.e_phnum);
		printf("\te_shentsize: %d\n", elf->header.e_shentsize);
		printf("\te_shnum: %d\n", elf->header.e_shnum);
		printf("\te_shstrndx: %d\n", elf->header.e_shstrndx);
	}
	if (flags & 0b10)
	{
		printf("ELF segments:\n");
		for (size_t i = 0; i < elf->header.e_phnum; i++)
		{
			printf("\tsegment:\n");
			printf("\t\tp_type 0x %x\n", elf->segments[i].p_type);
			printf("\t\tp_flags 0x%x\n", elf->segments[i].p_flags);
			printf("\t\tp_offset %lld\n", elf->segments[i].p_offset);
			printf("\t\tp_vaddr 0x%llx\n", elf->segments[i].p_vaddr);
			printf("\t\tp_paddr 0x%llx\n", elf->segments[i].p_paddr);
			printf("\t\tp_filesz %lld\n", elf->segments[i].p_filesz);
			printf("\t\tp_memsz %lld\n", elf->segments[i].p_memsz);
			printf("\t\tp_align %lld\n", elf->segments[i].p_align);
		}
	}
	if (flags & 0b001)
	{
		printf("ELF sections:\n");
		for (size_t i = 0; i < elf->header.e_shnum; i++)
		{
			printf("\t%s\n", elf64_get_section_name(elf, &elf->sections[i]));
			printf("\t\tsh_name: %d\n", elf->sections[i].sh_name);
			printf("\t\tsh_type: 0x%x\n", elf->sections[i].sh_type);
			printf("\t\tsh_flags: 0x%llx\n", elf->sections[i].sh_flags);
			printf("\t\tsh_addr: 0x%llx\n", elf->sections[i].sh_addr);
			printf("\t\tsh_offset: %lld\n", elf->sections[i].sh_offset);
			printf("\t\tsh_size: %lld\n", elf->sections[i].sh_size);
			printf("\t\tsh_link: 0x%x\n", elf->sections[i].sh_link);
			printf("\t\tsh_info: 0x%x\n", elf->sections[i].sh_info);
			printf("\t\tsh_addralign: 0x%llx\n", elf->sections[i].sh_addralign);
			printf("\t\tsh_entsize: %lld\n", elf->sections[i].sh_entsize);
		}
	}
}
