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
}

