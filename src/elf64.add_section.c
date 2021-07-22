/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.add_section.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:38:27 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/21 20:52:03 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>
#include <string.h>
#include <stdlib.h>
#include <elf.h>

int elf64_add_section(elf64_t *elf, char *content, size_t content_length)
{
    char        *new_data;
    // Elf64_Shdr  new_section;

    new_data = malloc(sizeof(char) * (elf->size + 0x40 + content_length));
	if (new_data == NULL)
        return (-1);
    for (size_t i = 0; i < elf->size; i++)
        new_data[i] = elf->data[i];

    *(Elf64_Word *)(new_data + elf->size + 0x00) = elf->sections[2].sh_name;
    *(Elf64_Word *)(new_data + elf->size + 0x04) = elf->sections[2].sh_type;
    *(Elf64_Xword *)(new_data +elf->size + 0x08) = 0x4;
    *(Elf64_Addr *)(new_data + elf->size + 0x10) = elf->size + 0x40;
    *(Elf64_Off *)(new_data + elf->size + 0x18)= elf->size + 0x40;
    *(Elf64_Xword *)(new_data + elf->size + 0x20) = content_length;
    *(Elf64_Word *)(new_data + elf->size + 0x28) = 0;
    *(Elf64_Word *)(new_data + elf->size + 0x2c) = 0;
    *(Elf64_Xword *)(new_data + elf->size + 0x30) = elf->sections[2].sh_addralign;
    *(Elf64_Xword *)(new_data + elf->size + 0x38) = 0;

    for (size_t i = 0; i < content_length; i++)
        new_data[elf->size + 0x40 + i] = content[i];

    elf->header.e_entry = *(Elf64_Addr *)(new_data + elf->size + 0x10);
    elf->header.e_shnum += 1;
    free(elf->data);
    elf->data = new_data;
    elf->size += 0x40 + content_length;
    
    *(Elf64_Half *)(new_data + 0x3c) = elf->header.e_shnum;
    *(Elf64_Addr *)(new_data + 0x18) = elf->header.e_entry;

    return (0);
}


