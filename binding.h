#ifndef CALC_ENVIRONMENT_BINDING_H
#define CALC_ENVIRONMENT_BINDING_H 

#include "value.h"

#include <stdlib.h>

#define T Binding
typedef struct T *T;

struct T {
    char *name;
    Value value;
    Binding rest;
};

static const T EMPTY_BINDING = NULL;

T Binding_new(char *name, Value v);

/* Free the entire chain of bindings */
void Binding_free(T *b);

T Binding_prepend(T new_b, T rest);
Value Binding_find(T list, char *name);

void Binding_print(T b);

#undef T
#endif

