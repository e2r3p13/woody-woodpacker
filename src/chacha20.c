/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chacha20.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 06:25:00 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 02:08:23 by bccyv            ###   ########.fr       */
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

#define QR(a, b, c, d)					\
(										\
	a += b,  d ^= a,  d = ROTL(d,16),	\
	c += d,  b ^= c,  b = ROTL(b,12),	\
	a += b,  d ^= a,  d = ROTL(d, 8),	\
	c += d,  b ^= c,  b = ROTL(b, 7)	\
)

int chacha20_keygen(t_key key)
{
	int urandfd;
	int	rdret;

	urandfd = open("/dev/urandom", O_RDONLY);
	if (urandfd < 0)
	{
		printf("Error: Failed to generate a key\n");
		return (-1);
	}
	rdret = read(urandfd, (char *)key, 32);
	close(urandfd);
	if (rdret < 32)
	{
		printf("Error: Failed to generate a key\n");
		return (-1);
	}
	return (0);
}

void chacha20_keyprint(t_key key)
{
	uint8_t *stream = (uint8_t *)key;

	printf("Chacha key:\n");

	for (int i = 0; i < 32; i++)
	{
		printf("%.2x%c", stream[i], i == 31 ? '\n' : ':');
	}
}

static void chacha_block(uint32_t working_state[16], t_key key, uint32_t counter)
{

	uint32_t state[16] =
	{
		0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
		key[0],         key[1],     key[2],     key[3],
		key[4],         key[5],     key[6],     key[7],
		counter,           0x0, 0x4a000000,        0x0
	};

	memcpy(working_state, state, sizeof(uint32_t) * 16);
	
	for (int i = 0; i < 10; i++)
	{
		QR(state[ 0], state[ 4], state[ 8], state[12]);
		QR(state[ 1], state[ 5], state[ 9], state[13]);
		QR(state[ 2], state[ 6], state[10], state[14]);
		QR(state[ 3], state[ 7], state[11], state[15]);

		QR(state[ 0], state[ 5], state[10], state[15]);
		QR(state[ 1], state[ 6], state[11], state[12]);
		QR(state[ 2], state[ 7], state[ 8], state[13]);
		QR(state[ 3], state[ 4], state[ 9], state[14]);
	}

	for (int i = 0; i < 16; i++)
		working_state[i] += state[i];
}

void chacha20_run(uint8_t *data, size_t len, t_key key)
{
	uint32_t    key_stream[16];
	uint32_t	counter = 0;

	for (size_t i = 0; i < len; i++)
	{
		if (i % 64 == 0)
		{
			counter++;
		    chacha_block(key_stream, key, counter);
		}
		data[i] ^= ((uint8_t *)key_stream)[i % 64];
	}
}
