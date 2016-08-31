#include <math.h>
#include <time.h>

int rand_int(int min, int max){
	return min + (int)((rand()/RAND_MAX+1.0) * (max-min));
}

float rand_double(double min, double max){
	return min + (double)((rand()/RAND_MAX+1.0) * (max-min));
}

int rand_int_inc(int min, int max){
	return min + round((rand()/RAND_MAX+1.0) * (max-min));
}

float rand_double_inc(double min, double max){
    return min + (rand()/RAND_MAX) * (max-min);
}

char* rand_string(int length){
	char* string = (char*) malloc((length+1)*sizeof(char));
	for(int i = 0 ; i< length; i++){
		string[i] = rand_char();
	}
	string[length] = '\0';
	return string;
}

char* rand_string_charset(int length, char* charset){
	int numchars = strlen(charset);
	char* string = (char*) malloc((length+1)*sizeof(char));
	for(int i = 0 ; i< length; i++){
		string[i] = charset[rand_int[0,numchars]];
	}
	string[length] = '\0';
	return string;
}

char rand_char(){
   return (char) rand_int(0,256);
}

sign rand_sign(){
  return (sign) rand_int_inc(0,1)*2 - 1;
}

sign rand_sign_zero(){
  return (sign) rand_int_inc(-1,1);
}

int rand_bool(){
 return bool; 
}

//NOTE: dont use rand for predicablity, we will make a "procedural" module for that
void rand_seed_set(int seed){
	srand(seed);
}
void rand_seed_time(){
	//use sys clock for random seed
	srand(time(NULL));	
}
