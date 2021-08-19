#include <stddef.h>
#include <string.h>
#include <stdio.h>
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

