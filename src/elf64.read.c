#include <elf.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <woody.h>

void elf64_free(Elf64 *elf)
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

static int read_header(Elf64 *elf, char *fdata)
{
    size_t hsize = sizeof(Elf64_Ehdr);

    memcpy(&elf->header, fdata, hsize);

    if (strncmp((char *)elf->header.e_ident, ELFMAG, SELFMAG) != 0)
        return (-1);
    if (elf->header.e_machine != 0x03)
        return (-1);

    return (0);
}

static int read_pheaders(Elf64 *elf, char *fdata)
{
    size_t phsize = sizeof(Elf64_Phdr) * elf->header.e_phnum;

    elf->pheaders = malloc(phsize);
    if (elf->pheaders == NULL)
        return (-1);

    memcpy(elf->pheaders, fdata + elf->header.e_phoff, phsize);

    return (0);
}

static int read_sheaders(Elf64 *elf, char *fdata)
{
    size_t shsize = sizeof(Elf64_Shdr) * elf->header.e_shnum;

    elf->sheaders = malloc(shsize);
    if (elf->pheaders == NULL)
        return (-1);

    memcpy(elf->sheaders, fdata + elf->header.e_shoff, shsize);

    return (0);
}

static int read_scontent(Elf64 *elf, char *fdata)
{
    Elf64_Shdr  *shdr;

    elf->scontent = malloc(sizeof(char *) * elf->header.e_shnum);
    if (elf->scontent == NULL)
        return (-1);

    for (size_t i = 0; i < elf->header.e_shnum; i++)
    {
        shdr = &elf->sheaders[i];
        elf->scontent[i] = malloc(sizeof(char) * shdr->sh_size);
        if (elf->scontent[i] == NULL)
            return (-1);
        memcpy(elf->scontent[i], fdata + shdr->sh_offset, shdr->sh_size);
    }
    return (0);
}

static char *read_file(const char *path, size_t *fsize)
{
	int		fd;
	char	*fdata;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (NULL);
	*fsize = lseek(fd, 0, SEEK_END);
	if ((off_t)*fsize == (off_t)-1)
	{
		close(fd);
		return (NULL);
	}
	lseek(fd, 0, SEEK_SET);

	fdata = malloc(sizeof(char) * (*fsize));
	if (!fdata)
	{
		close(fd);
		return (NULL);
	}
	if (read(fd, fdata, *fsize) < (ssize_t)*fsize)
	{
		close (fd);
		return (NULL);
	}
	close(fd);
	return (fdata);
}

Elf64   *elf64_read(char *fpath)
{
    Elf64   *elf;
	char	*fdata;
	size_t	fsize;
    size_t  minsize = sizeof(Elf64_Ehdr);

	fdata = read_file(fpath, &fsize);

	if (fdata == NULL)
		return(NULL);
    if (fsize < minsize)
        return (NULL);
    if ((elf = malloc(sizeof(Elf64))) == NULL)
        return (NULL);

    memset(elf, 0, sizeof(Elf64));

    if (read_header(elf, fdata) < 0)
    {
        elf64_free(elf);
        return (NULL);
    }

    minsize += sizeof(Elf64_Phdr) * elf->header.e_phnum + sizeof(Elf64_Shdr) * elf->header.e_shnum;
    if (fsize < minsize || read_pheaders(elf, fdata) < 0 || read_sheaders(elf, fdata) < 0)
    {
        elf64_free(elf);
        return (NULL);
    }

    for (size_t i = 0; i < elf->header.e_shnum; i++)
        minsize += elf->sheaders[i].sh_size;
    if (fsize < minsize || read_scontent(elf, fdata) < 0)
    {
        elf64_free(elf);
        return (NULL);
    }

    return (elf);
}
