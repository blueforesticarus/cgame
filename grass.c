#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
//this file explores to idea of rendering grass waves by simulating presure waves in in the air (wind)
//as this is now it may be useful for something else, but I dont think that wind rendering in this fasion can be done in a quick or effective way

//could also just implement a waze function, but it wouldnt look as nice
//if i get this to work there is going to be alot of wind and grass in this game

#define COUNT_WIND_NODES 10 //number of presure nodes
#define WIND_INTERPLAY .02	//coeficient of force between high and low pressure nodes
#define WIND_EFFECT_CAP 16 //no distances treated as closer than 3 when calculating grass sway
#define WIND_EFFECT_MULTIPLIER 1 //wind effect
#define DELAY 10000			//frame delay
#define CUTOFF 50

#define DEBUG_NODES true

int random_number(int min_num, int max_num);
float random_float(float min, float max);
int power(int base, unsigned int exp) ;
void update();

int run = true;
void killer(int dummy){
 run=0;
}

typedef struct {
  double x,y,sx,sy,s;
} windnode;

int maxx, maxy;
windnode nodes[COUNT_WIND_NODES];

int main(int arg , char *argc[]){
	int i,x,y;
	char *frame;

	signal(SIGINT, killer);

	initscr();
    noecho();
    cbreak();
    nodelay(stdscr, 1);
    curs_set(FALSE);
    start_color();

	init_color(20, 200, 700, 200);
    init_color(21, 50, 200, 50);
	init_color(22, 100, 600, 100);
    init_color(23, 50, 200, 50);
	init_color(24, 0, 500, 0);
    init_color(25, 50, 200, 50);
	
	init_pair(101, 20, 21);
	init_pair(102, 22, 23);		
	init_pair(103, 24, 25);		

	attron(COLOR_PAIR(101));
	attron(A_BOLD);

    getmaxyx(stdscr, maxy, maxx);
	frame = malloc(maxx * maxy * sizeof(char));

	for(i=0; i<COUNT_WIND_NODES; ++i){
		nodes[i].x = random_float(0, maxx);
		nodes[i].y = random_float(0, maxy);
		nodes[i].s = random_float(-100,100);
		nodes[i].sx = random_float(0, 2);
		nodes[i].sy = random_float(-.3,.3); 
	}


	while(run){
		for(y = 0 ; y < maxy ; ++y){
			for(x = 0 ; x < maxx ; ++x){
				double total = 0;
				double divisor = 0;
				//get average strength of wind (- for opposite direction) with weights base on square of distance
				for(int i=0; i<COUNT_WIND_NODES; ++i){
					double dx = nodes[i].x - x;
					double dy = nodes[i].y - y; 
					double distance_sqr= power(dx ,2) + power(dy,2);
					if(distance_sqr < WIND_EFFECT_CAP) distance_sqr = WIND_EFFECT_CAP;

					double m = 1;
					if(nodes[i].x < 0){
						m*= 1 + (nodes[i].x / CUTOFF);
					}else if(nodes[i].y < 0){
						m*= 1 + (nodes[i].y / CUTOFF);
					}

					if(nodes[i].x > maxx){
						m*= 1 - ((nodes[i].x-maxx) / CUTOFF);
					}else if(nodes[i].y > maxy){
						m*= 1 - ((nodes[i].y-maxy) / CUTOFF);
					}

					if (nodes[i].s>0){
						divisor += nodes[i].s/distance_sqr;
						total += nodes[i].sx * nodes[i].s/distance_sqr * m;	
					}else{
						divisor -= nodes[i].s/distance_sqr;
						total -= nodes[i].sx * nodes[i].s/distance_sqr * m;
					}
	
				}

				total /= divisor;
				total *= WIND_EFFECT_MULTIPLIER;

				if(total > 1){
					frame[x+(y*maxx)] = '/';

					attron(COLOR_PAIR(101));
					mvaddch(y,x,'/');
				}else if (total < -1){
					frame[x+(y*maxx)] = '\\';

					attron(COLOR_PAIR(103));
					mvaddch(y,x,'\\');
				}else{            
					frame[x+(y*maxx)] = '|';

					attron(COLOR_PAIR(102));
					mvaddch(y,x,'|');
				}
			}	
		}

		//mvaddstr(0,0,frame);
		
		attron(COLOR_PAIR(1));
		for(i=0; i<COUNT_WIND_NODES && DEBUG_NODES; ++i){
			if(nodes[i].s > 20){
				mvaddch(nodes[i].y, nodes[i].x, '+');
			}else if (nodes[i].s < -20){
				mvaddch(nodes[i].y, nodes[i].x, '-');
			}else{
				mvaddch(nodes[i].y, nodes[i].x, 'o');
			}
		}

		refresh();
		erase();
		update();
		usleep(DELAY);
	}

	attroff(COLOR_PAIR(101));
    endwin();

}

void update(){
	//update node placements each frame
	for(int i=0; i<COUNT_WIND_NODES; ++i){
		for(int o=0; o<COUNT_WIND_NODES; ++o){
			if(o==i) continue;
			double dx = nodes[o].x - nodes[i].x;
			double dy = nodes[o].y - nodes[i].y; 
			double distance_sqr= power(dx ,2) + power(dy,2);
			if(distance_sqr < .3) continue;
			double distance = sqrt(distance_sqr);
			double diff = nodes[i].s - nodes[o].s;
			double force = (diff / distance_sqr) * WIND_INTERPLAY / COUNT_WIND_NODES;
			nodes[i].sx += force * dx / distance;
			nodes[i].sy += force * dy / distance;	
		}

		float speed = .003 * maxx;
		nodes[i].x += nodes[i].sx * speed;
		nodes[i].y += nodes[i].sy * speed/2;

		if ( nodes[i].x > maxx + CUTOFF || nodes[i].y < -CUTOFF || nodes[i].y > maxy + CUTOFF || nodes[i].x < - (CUTOFF + 10) ){
			//off screen

			nodes[i].y = random_float(maxy/7.0, 6.0*maxy/7.0);
			nodes[i].s = random_float(-100,100);
			if (nodes[i].s > 0 || true){
				nodes[i].x = -CUTOFF;
				nodes[i].sx = random_float(.5, 2);
			}else{
				nodes[i].x = maxx + CUTOFF;
		        nodes[i].sx = random_float(-.5, -2);					
			} 

			nodes[i].sy = random_float(-.2, .2); 
		}
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
