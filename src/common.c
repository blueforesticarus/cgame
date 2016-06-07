
/* Random */
int rand_int(int min, int max){
	RAND_MAX
		
    int result=0,low_num=0,hi_num=0;
    if(min_num<max_num){
    	low_num=min;
    	hi_num=max;
    }else{
    	low_num=max;
    	hi_num=min;
    }
    result = (rand()%(hi_num-low_num))+low_num;
    return result;
}

float rand_float(float min, float max);
char* rand_string(int length);
char* rand_string_spec(int length, char* options);
char rand_char();
int rand_sign();


