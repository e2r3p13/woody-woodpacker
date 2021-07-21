/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfalkau <lfalkau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/17 02:40:05 by lfalkau           #+#    #+#             */
/*   Updated: 2021/07/20 18:57:18 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <elf.h>
#include <stdlib.h>
#include <stdio.h>

#include <woody.h>

int	ft_error(const char *errmsg)
{
	write(2, errmsg, strlen(errmsg));
	return (1);
}

int	main(int ac, char **av)
{
	char	*path = av[1];
	elf64_t	elf;

	if (ac < 2)
		return ft_error("Usage: ./woody <elf64 executable>\n");
	memset(&elf, 0, sizeof(elf64_t));
	if (elf64_parse(path, &elf) < 0)
		return ft_error("woody: Failed to parse av[1]. Make sure it does existes and is a valid elf64 file.\n");

	// elf64_print(&elf, 0b111);
	elf64_cleanup(&elf);

	return (0);
}
