#ifndef _STDDEF_H
#define _STDDEF_H
typedef unsigned long size_t;
typedef long ptrdiff_t;
#ifndef NULL
#define NULL 0
#endif
#define offsetof(type, member) ((size_t) & (((type *)0)->member))
#endif
