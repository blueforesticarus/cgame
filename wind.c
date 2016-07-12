#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define WIND_ROWS 20
#define WIND_COLUMNS 14
#define PRESURE_AIRSPEED_CONSTANT .2
#define WIND_MOMENTUM .2

#define DELAY 8000			//frame delay

typedef struct {
  double pressure;
  double fx, fy;
} WindNode;

int maxx, maxy;
WindNode nodes[WIND_ROWS*WIND_COLUMNS];

int frame;
int run = 1;
void killer(int dummy){
 run=0;
 endwin();
}

int random_number(int min_num, int max_num);
float random_float(float min, float max);
int power(int base, unsigned int exp) ;
void update();

void debug_printNodes(){
	char temp[50];
	int color;
	for(int y=0; y< WIND_ROWS; y++){
		for(int x=0; x< WIND_COLUMNS; x++){
			sprintf(temp,"%.3f", nodes[x + y*WIND_COLUMNS].fx);
			color = 120 + (int)(nodes[x + y*WIND_COLUMNS].fx * 7);
			if(color >= 140){
				color= 139;
			}else if(color<100){
				color= 100;
			}
			attron(COLOR_PAIR(color));
			mvaddstr(y*2 + 1,x*8+1,temp);
		}
	}
	refresh();
}

void update(){
	int row, col;
	double diff;
	frame++;

	//sinusoidal inflow from left
	for(int y = 0; y<WIND_ROWS; y++){
		nodes[y*WIND_COLUMNS].pressure = 60 + 40*sin(frame/35.0);
	}

	//calculate airflow
	for(int i=0; i < WIND_ROWS*WIND_COLUMNS; i++){
		if(i / WIND_COLUMNS != WIND_ROWS - 1){
			//verticle
			diff = nodes[i].pressure - nodes[i+WIND_COLUMNS].pressure;				
			nodes[i].fy = (WIND_MOMENTUM * nodes[i].fy) + ((1-WIND_MOMENTUM) * diff * PRESURE_AIRSPEED_CONSTANT);
		}

		if(i % WIND_COLUMNS != WIND_COLUMNS - 1){
			//horizontal	
			diff = nodes[i].pressure - nodes[i+1].pressure;				
			nodes[i].fx = (WIND_MOMENTUM * nodes[i].fx) + ((1-WIND_MOMENTUM) * diff * PRESURE_AIRSPEED_CONSTANT);
		}else{
			//right edge (emulate no presure for drain)
			diff = nodes[i].pressure;			
			nodes[i].fx = (WIND_MOMENTUM * nodes[i].fx) + ((1-WIND_MOMENTUM) * diff * PRESURE_AIRSPEED_CONSTANT);
		}
	}

	//sinusoidal inflow from left (THIS TEST OVERRIDES FIRST FOR LOOP)
	for(int y = 0; y<WIND_ROWS; y++){
	//	nodes[y*WIND_COLUMNS].fx += 1 * sin(frame/500.0)
	}

	//update pressure
	for(int i=0; i < WIND_ROWS * WIND_COLUMNS; i++){
		nodes[i].pressure -= nodes[i].fy + nodes[i].fx;
		if(i % WIND_COLUMNS > 0) nodes[i].pressure += nodes[i - 1].fx;
		if(i >= WIND_COLUMNS) nodes[i].pressure += nodes[i - WIND_COLUMNS].fy;
	}
}

int main(int arg , char *argc[]){
	signal(SIGINT, killer);

	initscr();
    noecho();
    cbreak();
    nodelay(stdscr, 1);
    curs_set(FALSE);
    start_color();

    getmaxyx(stdscr, maxy, maxx);

	init_color(1,1000,1000,1000);
	for(int i=0;i<40;i++){
		init_color(10+i, 25*i, 0, 25*(40-i));
		init_pair(100 + i, 1 , 10+ i);
	}

	for(int i=0; i < WIND_ROWS*WIND_COLUMNS; i++){
		nodes[i].pressure = random_number(0,100);
	}
	
	while(run){
		erase();
		update();
		debug_printNodes();	
		usleep(DELAY);
	}

}



int random_number(int min_num, int max_num)
{
    int result=0,low_num=0,hi_num=0;
    if(min_num<max_num)
    {
    low_num=min_num;
    hi_num=max_num; 
    }else{
    low_num=max_num;
    hi_num=min_num;
    }
    //srand(time(NULL));
    result = (rand()%(hi_num-low_num))+low_num;
    return result;
}
float random_float(float min_num, float max_num)
{
    float result=0,low_num=0,hi_num=0;
    if(min_num<max_num)
    {
    low_num=min_num;
    hi_num=max_num; 
    }else{
    low_num=max_num;
    hi_num=min_num;
    }
    //srand(time(NULL));
    result = ( (float) rand() / (float) RAND_MAX) * (hi_num-low_num) + low_num;
    return result;
}

int power(int base, unsigned int exp) {
    int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
}
