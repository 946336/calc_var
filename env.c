
#include "env.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

struct Env {
    Binding bindings;
    Env rest;
};

Env Env_new()
{
    Env e = malloc(sizeof(*e));
    if (e == NULL) {
        perror("Env_new");
        exit(EXIT_FAILURE);
    }
    e->bindings = NULL;
    e->rest = NULL;

    return e;
}

Env Env_new_extension(Env e)
{
    Env new_env = Env_new();
    new_env->bindings = NULL;
    new_env->rest = e;

    return new_env;
}

void Env_free(Env *e)
{
    if (e == NULL || *e == NULL) return;
    Env rest = (*e)->rest;
    Binding_free(&(*e)->bindings);
    free(*e);
    *e = rest;
}

void Env_free_r(Env *e)
{
    if (e == NULL || *e == NULL) return;
    Env_free_r(&(*e)->rest);
    Env_free(e);
    *e = NULL;
}

Value Env_find(Env e, char *name)
{
    if (e == NULL) {
        fprintf(stderr, "%s: %s\n", "Env_find", "Expected non-NULL "
                        "environment");
        return NOTHING;
    }

    Value v = Binding_find(e->bindings, name);
    if ((v.type != NONE) || (e->rest == NULL)) {
        return v;
    } else {
        return Env_find(e->rest, name);
    }

}

Env Env_bind(Env e, char *name, Value val)
{
    if (name == NULL) return e;
    if (e == NULL) return e;

    Binding tmp;

    switch (val.type) {
        case NONE: return e;
        case NUMBER:
        case STRING:
            tmp  = Binding_new(name, val);
            e->bindings = Binding_prepend(tmp, e->bindings);
            break;
        default:
            return Env_bind(e, val.u.name, Value_copy(Env_find(e, val.u.name)));
            // fprintf(stderr, "Value type not supported\n");
    }
    return e;
}

void Env_print(Env e)
{
    if (e == NULL) return;
    Binding_print(e->bindings);
    Env_print(e->rest);
}

