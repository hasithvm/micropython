#ifndef __MICROPY_INCLUDED_LIB_BASE64_H__
#define __MICROPY_INCLUDED_LIB_BASE64_H__

#include <stdlib.h>

#define base64_enclen(a) (((((a)*4)/3) + 3) & 0xFFFFFFFC)
#define base64_declen(a) (((a)*3)/4)

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

int base64decode (unsigned char *in, size_t inLen, char *out, size_t *outLen);
int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize);

#endif
