#ifndef WOODY_H
#define WOODY_H

#include <stddef.h>
#include <elf.h>
#include <chacha20.h>

typedef struct
{
    Elf64_Ehdr	header;
    Elf64_Phdr	*pheaders;
    Elf64_Shdr	*sheaders;
    uint8_t		**scontent;
}	Elf64;

Elf64   *elf64_read(char *fpath);
void	elf64_print(Elf64 *elf, int flags);
int		elf64_write(Elf64 *elf, const char *path);
void    elf64_free(Elf64 *elf);
int		elf64_inject_loader(Elf64 *elf, uint8_t *loader, size_t lsize);
char	*elf64_get_section_name(Elf64 *elf, size_t index);
void	elf64_encrypt_section(Elf64 *elf, const char *sname, Cha20Key key);
int		elf64_add_section(Elf64 *elf, char *sname, uint8_t *scontent, size_t ssize);

#endif
