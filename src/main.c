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

void elf64_print(elf64_t *elf)
{
    printf("e_ident: %s\n", elf->header.e_ident);
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
    elf64_print(&elf);
	return (0);
}
