#ifndef _MASC_CSTR_H_
#define _MASC_CSTR_H_

#include <stdlib.h>
#include <stdbool.h>

#include <masc/list.h>

size_t cstr_putc(char *dest, const char c, size_t size);

size_t cstr_ncopy(char *dest, const char *src, size_t size);

size_t cstr_escape(char *dest, const char *src, size_t src_len, size_t size);
size_t cstr_unescape(char *dest, const char *src, size_t size);

bool cstr_is_empty(const char *cstr);

bool cstr_eq(const char *cstr, const char *other);

size_t cstr_startswith(const char *cstr, const char *start);
size_t cstr_endswith(const char *cstr, const char *end);

char *cstr_lower(char *cstr);
char *cstr_upper(char *cstr);
char *cstr_swapcase(char *cstr);

List *cstr_split(const char *cstr, const char *sep, int maxsplit);

#endif /* _MASC_CSTR_H_ */
