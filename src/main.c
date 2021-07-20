/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfalkau <lfalkau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/17 02:40:05 by lfalkau           #+#    #+#             */
/*   Updated: 2021/07/17 03:47:10 by lfalkau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <elf.h>
#include <stdlib.h>
#include <stdio.h>

#include <woody.h>

int ft_error(const char *errmsg)
{
    write(2, errmsg, strlen(errmsg));
    return (1);
}

int elf64_parse_header(Elf64_Ehdr *header, const char *data, size_t size)
{
    char expected_ident[] = {0x7f, 'E', 'L', 'F', 2};

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

int elf64_parse_segment(Elf64_Phdr *header, const char *data, size_t offset)
{
    header->p_type = *(Elf64_Word *)(data + offset + 0x0);
    header->p_flags = *(Elf64_Word *)(data + offset + 0x04);
    header->p_offset = *(Elf64_Off *)(data + offset + 0x08);
    header->p_vaddr = *(Elf64_Addr *)(data + offset + 0x10);
    header->p_paddr = *(Elf64_Addr *)(data + offset + 0x18);
    header->p_filesz = *(Elf64_Xword *)(data + offset + 0x20);
    header->p_memsz = *(Elf64_Xword *)(data + offset + 0x28);
    header->p_align = *(Elf64_Xword *)(data + offset + 0x30);
    return (0);
}

int elf64_parse_section(Elf64_Shdr *header, const char *data, size_t offset)
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
    return (0);
}

int elf64_parse(const char *path, elf64_t *elf)
{
    int         fd;
    struct stat s;

    fd = open(path, O_RDWR);
	if (fd < 0 || fstat(fd, &s) < 0)
		return (-1);
    elf->size = s.st_size;
    elf->data = malloc(sizeof(char) * elf->size);
    if (elf->data == NULL)
        return (-1);
    if (read(fd, elf->data, elf->size) < 0)
		return (-1);
    close(fd);
    if (elf64_parse_header(&elf->header, elf->data, elf->size) < 0)
        return (-1);
    elf->segments = malloc(sizeof(Elf64_Phdr) * elf->header.e_phnum);
    if (elf->segments == NULL)
        return (-1);
    for (size_t i = 0; i < elf->header.e_phnum; i++)
    {   
        if (elf->size < elf->header.e_phoff + elf->header.e_phentsize * (i + 1))
            return (-1);
        elf64_parse_segment(&elf->segments[i], elf->data, elf->header.e_phoff + elf->header.e_phentsize * i);
    }
    elf->sections = malloc(sizeof(Elf64_Shdr) * elf->header.e_shnum);
    if (elf->sections == NULL)
        return (-1);
    for (size_t i = 0; i < elf->header.e_shnum; i++)
    {   
        if (elf->size < elf->header.e_shoff + elf->header.e_shentsize * (i + 1))
            return (-1);
        elf64_parse_section(&elf->sections[i], elf->data, elf->header.e_shoff + elf->header.e_shentsize * i);
    }
    return (0);
}

int xor_encrypt(char *bin, size_t offset, size_t len, const int key)
{
    for (size_t i = offset; i < len; i++)
    {
        bin[i] ^= key;
    }
    return (0);
}

void elf64_print(elf64_t *elf, int flags)
{
    if (flags & 0b1)
    {
        printf("ELF header:\n");
        printf("\te_ident: %s\n", elf->header.e_ident);
        printf("\te_type: 0x%x\n", elf->header.e_type);
        printf("\te_machine: 0x%x\n", elf->header.e_machine);
        printf("\te_version: 0x%x\n", elf->header.e_version);
        printf("\te_entry: 0x%lx\n", elf->header.e_entry);
        printf("\te_phoff: %ld\n", elf->header.e_phoff);
        printf("\te_shoff: %ld\n", elf->header.e_shoff);
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
            printf("\t\tp_offset %ld\n", elf->segments[i].p_offset);
            printf("\t\tp_vaddr 0x%lx\n", elf->segments[i].p_vaddr);
            printf("\t\tp_paddr 0x%lx\n", elf->segments[i].p_paddr);
            printf("\t\tp_filesz %ld\n", elf->segments[i].p_filesz);
            printf("\t\tp_memsz %ld\n", elf->segments[i].p_memsz);
            printf("\t\tp_align %ld\n", elf->segments[i].p_align);
        }
    }
    if (flags & 0b001)
    {
        printf("ELF sections:\n");
        for (size_t i = 0; i < elf->header.e_shnum; i++)
        {
            printf("\tsection:\n");
            printf("\t\tsh_name: %d\n", elf->sections[i].sh_name);
            printf("\t\tsh_type: 0x%x\n", elf->sections[i].sh_type);
            printf("\t\tsh_flags: 0x%lx\n", elf->sections[i].sh_flags);
            printf("\t\tsh_addr: 0x%lx\n", elf->sections[i].sh_addr);
            printf("\t\tsh_offset: %ld\n", elf->sections[i].sh_offset);
            printf("\t\tsh_size: %ld\n", elf->sections[i].sh_size);
            printf("\t\tsh_link: 0x%x\n", elf->sections[i].sh_link);
            printf("\t\tsh_info: 0x%x\n", elf->sections[i].sh_info);
            printf("\t\tsh_addralign: 0x%lx\n", elf->sections[i].sh_addralign);
            printf("\t\tsh_entsize: %ld\n", elf->sections[i].sh_entsize);
        }
    }
}

int main(int ac, char **av)
{
	char	    *path = av[1];
    elf64_t     elf;

	if (ac < 2)
        return ft_error("Usage: ./woody <elf64 executable>\n");
    memset(&elf, 0, sizeof(elf64_t));
    if (elf64_parse(path, &elf) < 0)
        return ft_error("woody: Failed to parse av[1]. Make sure it does existes and is a valid elf64 file.\n");

    elf64_print(&elf, 0b111);

    free(elf.data);
    free(elf.segments);
    free(elf.sections);
	
    return (0);
}
