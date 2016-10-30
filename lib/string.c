#include "string.h"

void* memcpy(void* dst, const void* src, size_t length)
{
	if (dst == src || length == 0)
	{
		return dst;
	}
	char* dst_ = dst;
	const char* src_ = src;
	while (length--)
	{
		*(dst_++) = *(src_++);
	}
	return dst;
}

void* memset(void* dst, int c, size_t n)
{
	char* dst_ = dst;
	while(n--)
	{
		*(dst_++) = c;
	}
	return dst;
}

char* strcpy(char *dst, const char *src)
{
    char *ret = dst;
    while ((*dst++ = *src++) != '\0') {}
    return ret;
}
