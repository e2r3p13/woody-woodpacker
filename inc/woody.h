#ifndef WOODY_H
#define WOODY_H

#include <stddef.h>
#include <elf.h>

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

char	*elf64_get_section_name(Elf64 *elf, Elf64_Shdr *section);
int		elf64_add_section(Elf64 *elf, char *sname, uint8_t *scontent, size_t ssize);

void    chacha20_encrypt(char *txt, size_t offset, size_t size, uint32_t key[16]);
void    chacha20_decrypt(char *txt, size_t offset, size_t size, uint32_t key[16]);
int		chacha20_generate_key(uint32_t key[8]);

#endif
