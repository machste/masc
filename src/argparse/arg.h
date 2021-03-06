#ifndef _MASC_ARGPARSE_ARG_H_
#define _MASC_ARGPARSE_ARG_H_

#include <masc/argparse.h>


typedef enum  {
    AP_TYPE_ARGUMENT,
    AP_TYPE_OPTION,
    AP_TYPE_UNKNOWN
} ApType;

typedef struct ApArg {
    Object;
    ApType type;
    char *name;
    union {
        char c;
        char cstr[2];
    } flag;
    char *metavar;
    bool required;
    int n;
    char *dfl;
    ap_type_cb type_cb;
    char *help;
} ApArg;


extern const class *ApArgCls;

ArgparseError aparg_parse_nargs(ApArg *self, const char *nargs);

const char *aparg_dest(ApArg *self);

void aparg_set_default(ApArg *self, const char *dfl);

Str *aparg_usage(ApArg *self);
Str *aparg_help(ApArg *self);

#endif /* _MASC_ARGPARSE_ARG_H_ */
