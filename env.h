#ifndef CALC_LOOKUP_ENVIRONMENT_H
#define CALC_LOOKUP_ENVIRONMENT_H 

#include "value.h"
#include "binding.h"

#define T Env
typedef struct T *T;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                   *
 * Lookup environments binding names to values. Environments may be  * 
 * chained in a linked list style.                                   *
 *                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

T    Env_new();
T    Env_new_extension(T e);
/* Free this environment and leave the rest */
void Env_free(T *e);
/* Recursively free all environments */
void Env_free_r(T *e);

Value Env_find(T e, char *name);
T     Env_bind(T e, char *name, Value val);

void Env_print(T e);

#undef T
#endif

