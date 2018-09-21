#include <stdio.h>
#include <string.h>

#include <masc/json.h>
#include <masc/map.h>
#include <masc/list.h>
#include <masc/str.h>
#include <masc/num.h>
#include <masc/int.h>
#include <masc/bool.h>
#include <masc/none.h>
#include <masc/cstr.h>
#include <masc/iter.h>
#include <masc/math.h>
#include <masc/regex.h>
#include <masc/macro.h>


static char *err2str[] = {
    [JSON_SUCCESS] = "ok",
    [JSON_ERROR_FATAL] = "Fatal parser error",
    [JSON_ERROR_INVAL] = "Invalid character inside JSON string",
    [JSON_ERROR_PART] = "Not a full JSON string, more bytes expected",
    [JSON_ERROR_ROOT] = "Invalid root object",
    [JSON_ERROR_MISSMATCH] = "Missmatch of '}' or ']'",
    [JSON_ERROR_EXTRA_DATA] = "Extra data detected after end of JSON string",
    [JSON_ERROR_COMMA] = "Missing comma detected",
    [JSON_ERROR_COLON] = "Missing colon detected",
    [JSON_ERROR_EXPECT_VALUE] = "Value was expected"
};


static bool is_valid_root(const void *root)
{
    const Class *rcls[] = {MapCls, ListCls, StrCls, NumCls, IntCls,
        BoolCls, NoneCls
    };
    const Class *root_cls = class_of(root);
    for (int i = 0; i < ARRAY_LEN(rcls); i++) {
        if (root_cls == rcls[i]) {
            return true;
        }
    }
    return false;
}

Json *json_new(const void *root)
{
    Json *self = malloc(sizeof(Json));
    json_init(self, root);
    return self;
}

void json_init(Json *self, const void *root)
{
    object_init(&self->obj, JsonCls);
    if (is_valid_root(root)) {
        self->root = new_copy(root);
        self->error = JSON_SUCCESS;
    } else {
        self->root = NULL;
        self->error = JSON_ERROR_ROOT;
    }
}

static void _vinit(Json *self, va_list va)
{
    void *root = va_arg(va, void *);
    json_init(self, root);
}

Json *json_new_cstr(const char *cstr)
{
    Json *self = malloc(sizeof(Json));
    json_init_cstr(self, cstr);
    return self;
}

void json_init_cstr(Json *self, const char *cstr)
{
    object_init(&self->obj, JsonCls);
    self->error = json_parse_to_obj(&self->root, cstr);
}

Json *json_new_copy(const Json *other)
{
    Json *self = malloc(sizeof(Json));
    json_init_copy(self, other);
    return self;
}

void json_init_copy(Json *self, const Json *other)
{
    object_init(&self->obj, JsonCls);
    self->root = new_copy(other->root);
    self->error = other->error;
}

void json_destroy(Json *self)
{
    if (self->root != NULL) {
        delete(self->root);
    }
}

void json_delete(Json *self)
{
    json_destroy(self);
    free(self);
}

bool json_is_valid(Json *self)
{
    return self->error == JSON_SUCCESS;
}

const char *json_err_msg(Json *self)
{
    return err2str[self->error];
}

JsonError json_parse(Json *self, const char *cstr)
{
    if(self->root != NULL) {
        delete(self->root);
    }
    return (self->error = json_parse_to_obj(&self->root, cstr));
}

void *json_get_node(Json *self, const char *key)
{
    void *node = self->root;
    Regex re = init(Regex, "[\\.\\[]");
    List *tokens = regex_split(&re, key, -1);
    destroy(&re);
    Iter *itr = new(Iter, tokens);
    for (Str *tok = next(itr); tok != NULL && node != NULL; tok = next(itr)) {
        int idx = iter_get_idx(itr);
        if (str_is_empty(tok)) {
            if (idx == 0) {
                continue;
            } else {
                node = NULL;
            }
        } else if (str_get_at(tok, -1) == ']' && isinstance(node, List)) {
            char *endptr;
            long list_idx = strtol(tok->cstr, &endptr, 10);
            if (*endptr == ']') {
                node = list_get_at(node, list_idx);
            } else {
                node = NULL;
            }
        } else if (isinstance(node, Map)) {
            node = map_get(node, tok->cstr);
        } else {
            node = NULL;
        }
    }
    delete(itr);
    list_delete(tokens);
    return node;
}

static size_t indent_cstr(int level, char *cstr, size_t size)
{
    int len = level * 2;
    if (level > 0 && size > len) {
        memset(cstr, ' ', len);
    }
    return len;
}

static size_t obj_pretty_cstr(void *obj, bool last, int level, char *cstr,
        size_t size)
{
    long len = 0;
    if (isinstance(obj, Map)) {
        len += cstr_ncopy(cstr + len, "{\n", max(0, size - len));
        level++;
        Iter *itr = new(Iter, obj);
        for (void *o = next(itr); o != NULL; o = next(itr)) {
            len += indent_cstr(level, cstr + len, max(0, size - len));
            len += snprintf(cstr + len, max(0, size - len), "\"%s\": ",
                    iter_get_key(itr));
            len += obj_pretty_cstr(o, iter_is_last(itr), level, cstr + len,
                    max(0, size - len));
        }
        if (last) {
            len += indent_cstr(--level, cstr + len, max(0, size - len));
            len += cstr_ncopy(cstr + len, "}\n", max(0, size - len));
        } else {
            len += indent_cstr(--level, cstr + len, max(0, size - len));
            len += cstr_ncopy(cstr + len, "},\n", max(0, size - len));

        }
        delete(itr);
    } else if (isinstance(obj, List)) {
        len += cstr_ncopy(cstr + len, "[\n", max(0, size - len));
        level++;
        Iter *itr = new(Iter, obj);
        for (void *o = next(itr); o != NULL; o = next(itr)) {
            len += indent_cstr(level, cstr + len, max(0, size - len));
            len += obj_pretty_cstr(o, iter_is_last(itr), level, cstr + len,
                    max(0, size - len));
        }
        if (last) {
            len += indent_cstr(--level, cstr + len, max(0, size - len));
            len += cstr_ncopy(cstr + len, "]\n", max(0, size - len));
        } else {
            len += indent_cstr(--level, cstr + len, max(0, size - len));
            len += cstr_ncopy(cstr + len, "],\n", max(0, size - len));
        }
        delete(itr);
    } else {
        len += repr(obj, cstr + len, max(0, size - len));
        if (last) {
            len += cstr_ncopy(cstr + len, "\n", max(0, size - len));
        } else {
            len += cstr_ncopy(cstr + len, ",\n", max(0, size - len));
        }
    }
    if (level == 0) {
        // Remove trailing newline
        len--;
        if (max(0, size - len) > 0) {
            cstr[len] = '\0';
        }
    }
    return len;
}

size_t json_pretty_cstr(Json *self, char *cstr, size_t size)
{
    if (is_valid_root(self->root)) {
        return obj_pretty_cstr(self->root, true, 0, cstr, size);
    } else {
        self->error = JSON_ERROR_ROOT;
        return 0;
    }
}

void json_pretty_print(Json *self)
{
    size_t size = json_pretty_cstr(self, NULL, 0) + 1;
    char *cstr = malloc(size);
    json_pretty_cstr(self, cstr, size);
    puts(cstr);
    free(cstr);
}

size_t json_repr(Json *self, char *cstr, size_t size)
{
    return snprintf(cstr, size, "<%s root: %s (%s) at %p>",
            name_of(self), name_of(self->root), err2str[self->error], self);
}

size_t json_to_cstr(Json *self, char *cstr, size_t size)
{
    return to_cstr(self->root, cstr, size);
}

static Class _JsonCls = {
    .name = "Json",
    .size = sizeof(Json),
    .vinit = (vinit_cb)_vinit,
    .init_copy = (init_copy_cb)json_init_copy,
    .destroy = (destroy_cb)json_destroy,
    .len = (len_cb)object_len,
    .cmp = (cmp_cb)object_cmp,
    .repr = (repr_cb)json_repr,
    .to_cstr = (to_cstr_cb)json_to_cstr,
    .iter_init = NULL,
};

const void *JsonCls = &_JsonCls;
