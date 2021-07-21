#ifndef WOODY_H
#define WOODY_H

#include <stddef.h>
#include <elf.h>

typedef struct
{
    Elf64_Ehdr  header;
    Elf64_Phdr  *segments;
    Elf64_Shdr  *sections;
    char        *data;
    size_t      size;
}   elf64_t;

void	elf64_cleanup(elf64_t *elf);
int		elf64_parse(const char *path, elf64_t *elf);
void	elf64_print(elf64_t *elf, int flags);
char	*elf64_get_section_name(elf64_t *elf, Elf64_Shdr *section);

void    chacha20_encrypt(char *txt, size_t offset, size_t size, uint32_t key[16]);
void    chacha20_decrypt(char *txt, size_t offset, size_t size, uint32_t key[16]);
int		chacha20_generate_key(uint32_t key[8]);

#endif
