/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.write.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 21:35:50 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 22:01:07 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <fcntl.h>
#include <woody.h>
#include <unistd.h>

static int padd(int fd, size_t dst, size_t *off)
{
    while (*off < dst)
    {
        if (write(fd, &(char){0x00}, 1) < 1)
            return (-1);
        *off += 1;
    }
    return (0);
}

static int write_header(Elf64 *elf, int fd, size_t *off)
{
    unsigned int hsize = sizeof(Elf64_Ehdr);

    if (write(fd, (uint8_t *)(&elf->header), hsize) < hsize)
        return (-1);
    *off += hsize;

    return (0);
}

static int write_pheaders(Elf64 *elf, int fd, size_t *off)
{
    unsigned int phsize = sizeof(Elf64_Phdr) * elf->header.e_phnum;
    
    if (padd(fd, elf->header.e_phoff, off) < 0)
        return (-1);
    if (write(fd, (uint8_t *)(elf->pheaders), phsize) < phsize)
        return (-1);
    *off += phsize;

    return (0);
}

static int write_scontents(Elf64 *elf, int fd, size_t *off)
{
    unsigned int csize;

    for (size_t i = 0; i < elf->header.e_shnum; i++)
    {   
        if (elf->sheaders[i].sh_type == SHT_NOBITS)
            continue ;
        csize = elf->sheaders[i].sh_size;
        if (padd(fd, elf->sheaders[i].sh_offset, off) < 0)
            return (-1);
        if (write(fd, (uint8_t *)elf->scontent[i], csize) < csize)
            return (-1);
        *off += csize;
    }

    return (0);
}

static int write_sheaders(Elf64 *elf, int fd, size_t *off)
{
    unsigned int shsize = sizeof(Elf64_Shdr) * elf->header.e_shnum;
    
    if (padd(fd, elf->header.e_shoff, off) < 0)
        return (-1);
    if (write(fd, (uint8_t *)(elf->sheaders), shsize) < shsize)
        return (-1);
    *off += shsize;

    return (0);
}

int	elf64_write(Elf64 *elf, const char *path)
{
    int fd;
    int wsum = 0;
    size_t off = 0;

    fd = open(path, O_WRONLY | O_CREAT, 0766);
    if (fd < 0)
        return (-1);

    wsum += write_header(elf, fd, &off);
    wsum += write_pheaders(elf, fd, &off);
    wsum += write_scontents(elf, fd, &off);
    wsum += write_sheaders(elf, fd, &off);

    close(fd);
    return (wsum);
}
