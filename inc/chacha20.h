/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chacha20.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 21:54:01 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/22 22:17:24 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHACHA20_H
#define CHACHA20_H

#include <stddef.h>

typedef uint32_t Cha20Key[8];

void	chacha20_encrypt(uint8_t *data, size_t offset, size_t size, Cha20Key key);
void	chacha20_decrypt(uint8_t *data, size_t offset, size_t size, Cha20Key key);
int		chacha20_keygen(Cha20Key key);

#endif
