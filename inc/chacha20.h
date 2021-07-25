/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chacha20.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 21:54:01 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 02:08:02 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHACHA20_H
#define CHACHA20_H

#include <stddef.h>

/*
 *	Describes a 256 bits key
*/
typedef uint32_t t_key[8];

/*
 *	chacha20_run actually encrypts @data, but as it's xor, it also decrypts it.
 *	Here comes the 1 Million dollar naming dilemma.
 *	You can send your ideas at bccyv@protonmail.com
*/
void	chacha20_run(uint8_t *data, size_t offset, size_t size, t_key key);

int		chacha20_keygen(t_key key);
void	chacha20_keyprint(t_key key);

#endif
