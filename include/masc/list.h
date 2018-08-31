#ifndef _MASC_LIST_H_
#define _MASC_LIST_H_

#include <masc/object.h>
#include <masc/iter.h>


typedef struct ListNode ListNode;

typedef struct {
    Object obj;
    ListNode *node;
} List;


extern const Class *ListCls;


List *list_new(void);
void list_init(List *self);

List *list_new_copy(List *other);
void list_init_copy(List *self, List *other);

void list_destroy(List *self);
void list_delete(List *self);

void list_append(List *self, void *obj);

Iter list_iter(List *self);

void list_for_each(List *self, void (*obj_cb)(void *));

size_t list_to_cstr(List *self, char *cstr, size_t size);


#endif /* _MASC_LIST_H_ */
