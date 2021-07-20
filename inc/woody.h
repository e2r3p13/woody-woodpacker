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

#endif
