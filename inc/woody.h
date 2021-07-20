#ifndef WOODY_H
#define WOODY_H

#include <elf.h>

typedef struct
{
    Elf64_Ehdr  file_header;
    Elf64_Phdr  *segments_headers;
    Elf64_Shdr  *sections_headers;
    char        *data;
    size_t      size;
}   elf64_t;

// int elf64_parse_file_header;
// int elf64_parse_segment_header;
// int elf64_parse_section_header;

#endif
