/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chacha20.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 06:25:00 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 22:19:44 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <chacha20.h>

#define ROTL(a,b)						\
(										\
	((a) << (b)) | ((a) >> (32 - (b)))	\
)

#define QR(a, b, c, d)		\
(							\
	b ^= ROTL(a + d, 7),	\
	c ^= ROTL(b + a, 9),	\
	d ^= ROTL(c + b,13),	\
	a ^= ROTL(d + c,18)		\
)

static void	chacha_block(uint32_t matrix[16], uint32_t cypher[16])
{
	for (int i = 0; i < 16; i++)
		cypher[i] = matrix[i];

	for (int i = 0; i < 10; i++)
	{
		QR(cypher[ 0], cypher[ 4], cypher[ 8], cypher[12]);
		QR(cypher[ 5], cypher[ 9], cypher[13], cypher[ 1]);
		QR(cypher[10], cypher[14], cypher[ 2], cypher[ 6]);
		QR(cypher[15], cypher[ 3], cypher[ 7], cypher[11]);

		QR(cypher[ 0], cypher[ 1], cypher[ 2], cypher[ 3]);
		QR(cypher[ 5], cypher[ 6], cypher[ 7], cypher[ 4]);
		QR(cypher[10], cypher[11], cypher[ 8], cypher[ 9]);
		QR(cypher[15], cypher[12], cypher[13], cypher[14]);
	}

	for (int i = 0; i < 16; i++)
		cypher[i] += matrix[i];
}

int	chacha20_keygen(Cha20Key key)
{
	int urandfd;
	int	rdret;

	urandfd = open("/dev/urandom", O_RDONLY);
	if (urandfd < 0)
		return (-1);
	rdret = read(urandfd, (char *)key, 32);
	close(urandfd);
	return (rdret == 32 ? 0 : -1);
}

void	chacha20_encrypt(uint8_t *data, size_t offset, size_t size, Cha20Key key)
{
	uint32_t    cypher[16];
	uint32_t    matrix[16] =
	{
		0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
		key[0], key[1], key[2], key[3],
		key[4], key[5], key[6], key[7],
		0x0000, 0x0000, 0x0042, 0x0042
	};

	for (size_t i = 0; i < size; i++)
	{
		if (i % 64 == 0)
		{
		    *(uint64_t *)(matrix + 12) += 1;
		    chacha_block(matrix, cypher);
		}
		data[offset + i] ^= ((unsigned char *)cypher)[i % 64];
	}
}

void	chacha20_decrypt(uint8_t *data, size_t offset, size_t size, Cha20Key key)
{
	chacha20_encrypt(data, offset, size, key);
}
