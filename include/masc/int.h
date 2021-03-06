#ifndef _MASC_INT_H_
#define _MASC_INT_H_

#include <stdbool.h>

#include <masc/object.h>


typedef struct {
    Object;
    long val;
} Int;


extern const class *IntCls;


Int *int_new(long val);
void int_init(Int *self, long val);

Int *int_new_cstr(const char *cstr, bool strict);
void int_init_cstr(Int *self, const char *cstr, bool strict);

void int_delete(Int *self);

long int_get(Int *self);
void int_set(Int *self, long value);
bool int_set_cstr(Int *self, const char *cstr, bool strict);
long int_iadd(Int *self, Int *other);
Int *int_add(Int *self, Int *other);

bool int_in_range(Int *self, long start, long stop);

int int_cmp(const Int *self, const Int *other);

size_t int_to_cstr(Int *self, char *cstr, size_t size);

#endif /* _MASC_INT_H_ */
