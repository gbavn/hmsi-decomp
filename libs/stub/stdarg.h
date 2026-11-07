#ifndef _STDARG_H
#define _STDARG_H
typedef char *va_list;
#define va_start(ap, parm) ((ap) = (char *)(&(parm) + 1))
#define va_arg(ap, type) (*(type *)(((ap) += sizeof(type)) - sizeof(type)))
#define va_end(ap) ((void)0)
#endif
