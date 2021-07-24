/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.read.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/24 04:01:44 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 04:54:08 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <woody.h>
#include <stdio.h>

/*
 * Those two variables are used to check if the given file is large enough to
 *	contain the information it pretends to.
 *	has to be checked in order to memcpy the structures safely.
*/
static long fsize;
static long minsize = 0;

void elf64_free(t_elf *elf)
{
    if (elf != NULL)
    {
        if (elf->pheaders != NULL)
            free(elf->pheaders);
        if (elf->sheaders != NULL)
            free(elf->sheaders);
        if (elf->scontent != NULL)
        {
            for (size_t i = 0; i < elf->header.e_shnum; i++)
                if (elf->scontent[i] != NULL)
                    free(elf->scontent[i]);
            free(elf->scontent);
        }
        free(elf);
    }
}

/*
 *	To be packed, the given file must be:
 *	- A valid ELF file
 *	- 64 bit format
 *	- executable
 *	If an invalid file is processed, this function is guilty.
*/
static int read_header(t_elf *elf, char *fdata)
{
    size_t hsize = sizeof(Elf64_Ehdr);

	minsize += hsize;
	if (fsize < minsize)
		printf("Error: Invalid ELF file\n");

    memcpy(&elf->header, fdata, hsize);

    if (strncmp((char *)elf->header.e_ident, ELFMAG, SELFMAG) != 0)
	{
		printf("Error: Invalid ELF file\n");
		return (-1);
	}
	if (elf->header.e_ident[EI_CLASS] != 2)
	{
		printf("Error: 32 bit ELF files are not supported\n");
		return (-1);
	}
	if (elf->header.e_type != ET_EXEC)
	{
		printf("Error: Only executables are supported\n");
		return (-1);
	}

    return (0);
}

static int read_pheaders(t_elf *elf, char *fdata)
{
    size_t phsize = sizeof(Elf64_Phdr) * elf->header.e_phnum;

	minsize += phsize;
	if (fsize < minsize)
		printf("Error: Corrupted ELF file\n");

    if ((elf->pheaders = malloc(phsize)) == NULL)
	{
		perror("Error:");
		return (-1);
	}
    memcpy(elf->pheaders, fdata + elf->header.e_phoff, phsize);

    return (0);
}

static int read_sheaders(t_elf *elf, char *fdata)
{
    size_t shsize = sizeof(Elf64_Shdr) * elf->header.e_shnum;

	minsize += shsize;
	if (fsize < minsize)
		printf("Error: Corrupted ELF file\n");

    if ((elf->sheaders = malloc(shsize)) == NULL)
	{
		perror("Error:");
		return (-1);
	}
    memcpy(elf->sheaders, fdata + elf->header.e_shoff, shsize);

    return (0);
}

static int read_scontent(t_elf *elf, char *fdata)
{
    Elf64_Shdr  *shdr;

    if ((elf->scontent = malloc(sizeof(char *) * elf->header.e_shnum)) == NULL)
	{
		perror("Error:");
		return (-1);
	}

    for (size_t i = 0; i < elf->header.e_shnum; i++)
    {
        shdr = &elf->sheaders[i];

		minsize += shdr->sh_size;
		if (fsize < minsize)
			printf("Error: Corrupted ELF file\n");

        elf->scontent[i] = malloc(sizeof(char) * shdr->sh_size);
        if (elf->scontent[i] == NULL)
		{
			perror("Error:");
			return (-1);
		}
        memcpy(elf->scontent[i], fdata + shdr->sh_offset, shdr->sh_size);
    }
    return (0);
}

static char *read_file(const char *path)
{
	int		fd;
	char	*fdata;

	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		printf("Error: Make sure '%s' exists and can be read\n", path);
		return (NULL);
	}
	if ((fsize = get_file_size(fd)) < 0)
	{
		perror("Error");
		return (NULL);
	}
	fdata = malloc(sizeof(char) * fsize);
	if (fdata == NULL || read(fd, fdata, fsize) < fsize)
	{
		close(fd);
		perror("Error");
		return (NULL);
	}
	close(fd);
	return (fdata);
}

t_elf *elf64_read(char *fpath)
{
    t_elf	*elf;
	char	*fdata;

	if ((fdata = read_file(fpath)) == NULL)
		return(NULL);

    if ((elf = malloc(sizeof(t_elf))) == NULL)
	{
		perror("Error:");
		return (NULL);
	}
    memset(elf, 0, sizeof(t_elf));

    if (read_header(elf, fdata) < 0 ||
		read_pheaders(elf, fdata) < 0 ||
		read_sheaders(elf, fdata) < 0 ||
		read_scontent(elf, fdata) < 0)
    {
        elf64_free(elf);
        return (NULL);
    }
    return (elf);
}
