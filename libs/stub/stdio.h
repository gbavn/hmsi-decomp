#ifndef _STDIO_H
#define _STDIO_H
#include <stdarg.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int sprintf(char *, const char *, ...);
int vsnprintf(char *, size_t, const char *, va_list);
#ifdef __cplusplus
}
#endif
#endif
