// Common prototypes for functions used in this project.

#ifndef COMMON_H
#define COMMON_H

#include "datatypes.h"

/* Random */
int rand_int(int min, int max);
float rand_float(float min, float max);
char* rand_string(int length);
char* rand_string_spec(int length, char* options);
char rand_char();
int rand_sign();

#endif
