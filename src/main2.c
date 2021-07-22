/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:40:55 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 15:45:08 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int main(int ac, char **av)
{
	Elf64	*elf;

	if (ac != 2)
	{
		printf("Usage: woody_woodpacker <elf64 executable>\n");
		return (1);
	}
	elf = elf64_read(av[1]);
	if (elf == NULL)
	{
		printf("%s: Error, please make sure %s exists and is a valid elf64 file\n", av[0], av[1]);
		return (1);
	}
}
