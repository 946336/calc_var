
#include "binding.h"
#include "utility.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>


/****************************************************************************/

Binding Binding_new(char *name, Value v)
{
    if (name == NULL) return EMPTY_BINDING;

    Binding nb = malloc(sizeof(*nb));
    if (nb == NULL) {
        perror("Binding_new");
        exit(EXIT_FAILURE);
    }
    nb->value = Value_copy(v);
    nb->name = copy_string(name);
    nb->rest = EMPTY_BINDING;

    return nb;
}

void Binding_free(Binding *b)
{
    if ((b == NULL) || (*b == EMPTY_BINDING)) return;
    Binding_free(&((*b)->rest));
    free((*b)->name);
    Value_free(&(*b)->value);
    free(*b);
    *b = EMPTY_BINDING;
}

Binding Binding_prepend(Binding new_b, Binding rest)
{
    if (new_b == EMPTY_BINDING) return rest;
    new_b->rest = rest;
    return new_b;
}

Value Binding_find(Binding list, char *name)
{
    if (list == EMPTY_BINDING) return NOTHING;
    if (strcmp(name, list->name) == 0) return list->value;
    else return Binding_find(list->rest, name);
}

void Binding_print(Binding b)
{
    if (b == NULL) return;
    
    switch (b->value.type) {
        case NUMBER:
            fprintf(stdout, "[%s] --> [%.15g]\n", b->name, b->value.u.d);
            break;
        case STRING:
            fprintf(stdout, "[%s] --> [%s]\n", b->name, b->value.u.s);
            break;
        case BOOL:
            fprintf(stdout, "[%s] --> [%s]\n", b->name,
                            b->value.u.b ? "<True>" : "<False>");
            break;
        case VAR:
            fprintf(stdout, "[%s] --> [%s]\n", b->name, b->value.u.name);
            break;
        case NONE:
        case INVALID:
        case OP:
        case RELAT_OP:
            break;
    }
    Binding_print(b->rest);
}

/****************************************************************************/

