#ifndef OS_STRING_H_
#define OS_STRING_H_

#include "type.h"

void* memcpy(void* dst, const void* src, size_t length);
void* memset(void* dst, int c, size_t n);
char* strcpy(char *dst, const char *src);

#endif //OS_STRING_H_
