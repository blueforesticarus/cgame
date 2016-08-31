// Generic functions and structs

#ifndef GENERIC_H
#define GENERIC_H

//VECTOR
//TODO, just do 2d double for now, everything else later

typedef struct{
    double x;
    double y;
} vec2;

vec2 V(double x, double y);

vec2 vec2_scale(vec2 vec, double factor);
vec2 vec2_sum(vec2 vec_a, vec2 vec_b);
vec2 vec2_diff(vec2 vec_a, vec2 vec_b);
vec2 vec2_neg(vec2 vec);
vec2 vec2_inv(vec2 vec);
vec2 vec2_abs(vec2 vec);

void vec2_add(vec2* vec_target, vec2 vec_other);
void vec2_sub(vec2* vec_target, vec2 vec_other);
void vec2_floor(vec2* vec);

double vec2_distance(vec2 vec_a, vec2 vec_b);
double vec2_sqrdist(vec2 vec_a, vec2 vec_b);

//RANDOM
int rand_int(int min, int max);
float rand_double(double min, double max);
int rand_int_inc(int min, int max);
float rand_double_inc(double min, double max);
char* rand_string(int length);
char* rand_string_charset(int length, char* charset);
char rand_char();
sign rand_sign();
sign rand_sign_zero(){
bool rand_bool();

//NOTE: dont use rand for predicablity, we will make a "procedural" module for that
void rand_seed_set(int seed);
void rand_seed_time();//use sys clock for random seed

//STRING



//OTHER
typedef uint8_t byte;
typedef enum {
   false = 0,
   true = 1
} bool;

typedef enum {
   pos = 1,
   neg = -1,
   zero = 0
} sign;

#endif
