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
}	t_elf;

t_elf   *elf64_read(char *fpath);
void	elf64_print(t_elf *elf, int flags);
int		elf64_write(t_elf *elf, const char *path);
void    elf64_free(t_elf *elf);
int		elf64_inject_loader(t_elf *elf, uint8_t *loader, size_t lsize);
int		elf64_inject(t_elf *elf, uint8_t *loader, size_t lsize);
char	*elf64_get_section_name(t_elf *elf, size_t index);
int		elf64_encrypt_section(t_elf *elf, const char *sname, Cha20Key key);
long	get_file_size(int fd);

#endif
