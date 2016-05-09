#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>

#include <ncurses.h>

int run=1;
int maxx, maxy;

void draw_path(int* points, int num_points);
void gen_tenticle(int xoffset);
void init_colors();

void killer(int dummy){
	run=0;
}

int random_number(int min_num, int max_num)
{
    int result=0,low_num=0,hi_num=0;
    if(min_num<max_num)
    {
    low_num=min_num;
    hi_num=max_num+1; // this is done to include max_num in output.
    }else{
    low_num=max_num+1;// this is done to include max_num in output.
    hi_num=min_num;
    }

    result = (rand()%(hi_num-low_num))+low_num;
    return result;
}

int abs(int n) { return n > 0 ? n : -n; }
int absf(float n) { return n > 0 ? n : -n; }
int signi(int n) {return (n > 0) - (n < 0);}
int signf(float n) {return (n > 0) - (n < 0);}

int main(int argc, char *argv[]) {
	signal(SIGINT, killer);
   srand(time(NULL)); //set random seed

	initscr();
   noecho();
   cbreak();
   nodelay(stdscr, 1);
   curs_set(FALSE);
	init_colors();

	//get term dimensions
   getmaxyx(stdscr, maxy, maxx);
	
	for(int x = 10; x < maxx-10; x+=5){
      attron(COLOR_PAIR(random_number(1,99)));
		gen_tenticle(x);
	}

	usleep(2000000);
	endwin();
}

void gen_tenticle(int xoffset){
	//groups of two values: 1st is y, 2nd is x
	int points[1000];
	points[1]=xoffset;
	int num = 1;
	int dy = 1;
	int dx = 0;

	while( !(dy==1 && dx==0) || num<10){
		if(!run) break;

 		int lasty = points[num*2-2];
		int lastx = points[num*2-1];
		int length = random_number(2,6);
		
		//if dy is 0, switch dy
		//if dx is 0, switch dx
		//if neither are zero, set a random one to 0
		if(dy==0){
			dy = 1 - 2*(random_number(0,100)>75); 
		}
		else if(dx==0){	
			dx = 1 - 2*random_number(0,1); 
		}
		else if(dx && dy){
			if(random_number(0,1))
				//make it more likely that tenticles turn horzontal
				//when they are going down				
				if(dy<0 && random_number(0,1))
					dy=0;
				else
					dx=0;
			else
				dy=0;
		}

		points[num*2]   =	lasty+ dy*length;
		points[num*2+1] =	lastx+ dx*length;

		draw_path(points, num+1);	
		refresh();
		usleep(20000);

		num++;
	}
}

void draw_path(int* points, int num_points){
	int cy = points[0];
	int cx = points[1];			

	int t = 0;
	int tx = cx;
	int ty = cy;
	float slope;//slope from last point to target
	int undef;//slope is zero or undef.

	//draw lines between points
	while(t<num_points){
		int dy = ty-cy;
		int dx = tx-cx;
		float offset = cy-(slope*(cx-tx)+ty );
		char c;
				
		if(dx==0 && dy==0){
			t++;
			ty = points[t*2];
			tx = points[t*2+1];
			if(tx-cx==0 || ty-cy==0){
				undef = true;
			}else{
				slope = (float)(ty-cy)/(tx-cx);
				undef = false;	
			}
		}


		if(offset==0 || undef){
			cy += signi(dy);
			cx += signi(dx);
		}else if((offset*signi(dx)*signf(slope))>0){
			cx += signi(dx);
		}else if((offset*signi(dx)*signf(slope))<0){
			cy += signi(dy);
		}			

		if(!dx && !dy)
			c='o';
		else if(!dx)
			c='|';
		else if(!dy)
			c='-';
		else if(slope==1)
			c='/';
		else if(slope==-1)
			c='\\';	
		else if(absf(slope)<1)	
			c='-';
		else if(absf(slope)>1)
			c='|';
		else 
			c='?';
	
		mvaddch(maxy-cy,cx, c);
	}

}

void init_colors(){
    start_color();
	init_color(100,0,0,0);
	for(int i=1;i<100;i++){
   	init_color(i,1000*i/100,1000,1000*(100-i)/100);
   	init_pair(i,i,100);
	}
}



