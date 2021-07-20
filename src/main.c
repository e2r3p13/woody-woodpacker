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

int ft_error(const char *errmsg)
{
    write(2, errmsg, strlen(errmsg));
    return (1);
}

int parse_elf64_file_header(Elf64_Ehdr *header, const char *binary)
{
    memcpy(header->e_ident, binary, EI_NIDENT);
    if (header->e_ident[0] != 0x7f || memcmp(&(header->e_ident[1]), "ELF", 3) != 0)
        return (-1);
    header->e_entry = *(Elf64_Addr *)(binary + 0x18);
    header->e_phoff = *(Elf64_Off *)(binary + 0x20);
    header->e_shoff = *(Elf64_Off *)(binary + 0x28);
    header->e_ehsize = *(Elf64_Half *)(binary + 0x34);
    header->e_phentsize = *(Elf64_Half *)(binary + 0x36);
    header->e_phnum = *(Elf64_Half *)(binary + 0x38);
    header->e_shentsize = *(Elf64_Half *)(binary + 0x3a);
    header->e_shnum = *(Elf64_Half *)(binary + 0x3c);
    header->e_shstrndx = *(Elf64_Half *)(binary + 0x3e);
    return (0); 
}

int parse_elf64_program_header(Elf64_Phdr *header, const char *binary, size_t offset)
{
    
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

int main(int ac, char **av)
{
	char			*filename = av[1];
	char			*binary;
	int				fd;
	struct stat		s;
    Elf64_Ehdr      header = {0};

	if (ac < 2)
        return ft_error("Usage: ./woody <elf64 executable>\n");
	fd = open(filename, O_RDWR);
	if (fd < 0 || fstat(fd, &s) < 0)
		return ft_error("woody: Error while opening the executable.\n");
    binary = malloc(sizeof(char) * (s.st_size + 1));
    if (!binary)
        return ft_error("woody: Fatal error.\n");
    if (read(fd, binary, s.st_size) < 0)
		return ft_error("woody: Error while reading the executable.\n");
    close(fd);
    // 0x40 is the expected size for an elf64 header
    if (s.st_size < 0x40 || parse_elf64_file_header(&header, binary) < 0)
        return ft_error("woody: The provided file isn't a valid elf64 executable\n");

    return (0);
}
