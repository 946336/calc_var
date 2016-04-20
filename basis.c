
#include "basis.h"
#include "value.h"

#include <math.h>
#include <stdbool.h>

/****************************************************************************/

#define PI           3.14159265358979323846
#define EULER_N      2.7182818284590452353602874

/****************************************************************************/

Env add_basis(Env e)
{
    if (e == NULL) return NULL;

    double phi = (1 + sqrt((double) 5)) / 2;

    e = Env_bind(e, "PI",  Value_new_number(PI));
    e = Env_bind(e, "E",   Value_new_number(EULER_N));
    e = Env_bind(e, "PHI", Value_new_number(phi));

    e = Env_bind(e, "True",  Value_new_bool(true));
    e = Env_bind(e, "False", Value_new_bool(false));

    return e;
}