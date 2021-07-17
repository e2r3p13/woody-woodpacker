/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfalkau <lfalkau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/17 02:40:05 by lfalkau           #+#    #+#             */
/*   Updated: 2021/07/17 03:47:10 by lfalkau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int ac, char **av)
{
	char			*filename = av[1];
	char			*binary;
	int				fd;
	struct stat		s;

	if (ac < 2)
	{
		//error
		return (1);
	}
	fd = open(filename, O_RDWR);
	if (fd < 0 || fstat(fd, &s) < 0)
	{
		//error
		return (1);
	}
	binary = mmap(NULL, s.st_size, PROT_EXEC | PROT_READ | PROT_WRITE,  MAP_SHARED, fd, 0);
	if (binary == NULL)
	{
		//error
		return (1);
	}
	// if (!is_elf64(binary))
	// {
	// 	//error
	// 	return (1);
	// }
	return (0);
}
