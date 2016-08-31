#include "generic.h"
#include <math.h>

//returns a new vector
vec2 V(double x, double y){
	vec2 vector;
	vector.x = x;
	vector.y = y;
	return vector;
}

vec2 vec2_scale(vec2 vec, double factor){
	vec.x *= factor;
	vec.y *= factor;
	return vector;
}

vec2 vec2_sum(vec2 vec_a, vec2 vec_b){
	vec_a.x += vec_b.x;
	vec_a.y += vec_b.y;
	return vector;
}
vec2 vec2_diff(vec2 vec_a, vec2 vec_b){
	vec_a.x -= vec_b.x;
	vec_a.y -= vec_b.y;
	return vector;
}
vec2 vec2_neg(vec2 vec){
	vec.x = -vec.x;
	ve.y = -vec.y;
	return vector;
}
vec2 vec2_inv(vec2 vec){
	vec.x = 1.0/vec.x;
	vec.y = 1.0/vec.y;
	return vec;
}

vec2 vec2_abs(vec2 vec){
	vec.x = ( vec.x >= 0 ) ? vec.x : -vec.x; 
	vec.y = ( vec.y >= 0 ) ? vec.y : -vec.y; 
	return vec;
}

//returns distance between vectors
double vec2_distance(vec2 vec_a, vec2 vec_b){
	return sqrt(vec2_sqrdist(vec_a, vec_b));
}

//returns distance squared between vectors
double vec2_sqrdist(vec2 vec_a, vec2 vec_b);{
	return (vec_a.x-vec_b.x)*(vec_a.x-vec_b.x) + (vec_a.y-vec_b.y)*(vec_a.y-vec_b.y);
}

////The following three functions modify an 
////existing vector supplied by a referance

//adds other vector to the source vector
void vec2_add(vec2* vec_target, vec2 vec_other){
	vec_target->x += vec_other.x;
	vec_target->y += vec_other.y;
}

//subtracts other vector from the source vector
void vec2_sub(vec2* vec_target, vec2 vec_other){
	vec_target->x -= vec_other.x;
	vec_target->y -= vec_other.y;
}

//floors each component of vec
void vec2_floor(vec2* vec){
	vec_target->x = (int) vec_target->x;
	vec_target->y = (int) vec_target->y;
}

