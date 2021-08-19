#ifndef WOODY_H
#define WOODY_H

#include <stddef.h>
#include <stdbool.h>
#include <elf.h>
#include <chacha20.h>

/*
 *	Describes an elf executable.
 *	Every function of this project uses it, don't underestimate it.
*/
typedef struct
{
    Elf64_Ehdr	header;
    Elf64_Phdr	*pheaders;
    Elf64_Shdr	*sheaders;
    uint8_t		**scontent;
}	t_elf;

/*
 *	Main funcions of the packer, sorted from first to last used.
*/

t_elf   *elf64_read(char *fpath);
bool	elf64_is_packable(t_elf *elf);
int		elf64_encrypt_section(t_elf *elf, const char *sname, t_key key, uint32_t *txtsecsz);
int		elf64_inject(t_elf *elf, uint8_t *loader, size_t lsize);
int		elf64_write(t_elf *elf, const char *path);
void    elf64_free(t_elf *elf);

/*
 *	Side functions, some of them are needed, some are not.:
 *	Wanna play russian roulette? Delete one of them and see if it still
 *	compiles.
*/

void	elf64_print(t_elf *elf, int flags);
char	*elf64_get_section_name(t_elf *elf, size_t index);
long	get_file_size(int fd);

/*
 *	Utils functions (mainly copies of standard functions.
*/

size_t memmem(const void *haystack, size_t hlen, const char *needle, size_t nlen);

#endif
