#include <stddef.h>
#include <string.h>
#include <stdio.h>

/*
 * Searches @needle in @haystack.
 * @needle and @haystacks are memory pointers, thus the lenght is required.
 * Returns the offset from @haystack to the first @needle byte if a match
 * is found, 0 otherwise.
*/
size_t memmem(const void *haystack, size_t hlen, const char *needle, size_t nlen)
{
	int			match;
	const char	*hstring;

	hstring = (const char *)haystack;
	for (size_t i = 0; i < hlen; i++)
	{
		if (i + nlen > hlen)
			break ;
		match = 1;
		for (size_t j = 0; j < nlen; j++)
		{
			if (hstring[i + j] != needle[j])
			{
				match = 0;
				break ;
			}
		}
		if (match != 0)
			return (i);
	}
	return (0);
}

/*
 * Copy @n first bytes of @src in @dst.
*/
void *ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;

	if (!dst && !src)
		return (NULL);
	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	while (n--)
		*d++ = *s++;
	return (dst);
}

/*
 * Compares @s1 and @s2.
 * Returns 0 if they're the same, a non zero value otherwise.
*/
int ft_strcmp(const char *s1, const char *s2)
{
	unsigned long i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

/*
 * Compares the first @n bytes of @s1 and @s2.
 * Returns 0 if they're the same, a non zero value otherwise.
*/
int ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned long i;

	i = 0;
	while (s1[i] && s2[i] && i < n && s1[i] == s2[i])
		i++;
	return (i == n ? 0 : ((unsigned char)s1[i] - (unsigned char)s2[i]));
}

/*
 * Fills the first @n bytes of @dst with @c bytes.
*/
void *ft_memset(void *dst, int c, size_t n)
{
	unsigned char	*tmp;
	unsigned char	chr;

	tmp = (unsigned char *)dst;
	chr = (unsigned char)c;
	while (n--)
		*tmp++ = chr;
	return (dst);
}
