#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <assert.h>
#include <ncurses.h>

#define MAX_TENTICLE_LENGTH 100


int run=1;
int maxx, maxy;

typedef struct{
    int x;
    int y;
} vec2;

typedef struct{
	int length;//number of segments (note this means there are length+1 values in points)
	vec2 points[MAX_TENTICLE_LENGTH];//has length+1 elements

	int segments[MAX_TENTICLE_LENGTH];//lengths of each of the segments
	int joints[MAX_TENTICLE_LENGTH];//1=clockwise, -1=counterclockwise, 0=same direction
	vec2 direction;//initial direction
	vec2 position;//initial position
	int color; //color in ncurses to draw with

	int inflection; //point of inflection in tenticle
	int first_curl;//initial direction of curl (changes at tent.inflection)
	int delta;	   //the change in inflection point to for the next frame
} tenticle;


void draw_path(vec2* points, int num_points);
tenticle gen_tenticle(int xoffset, int yoffset);
void render_tenticle(tenticle tent);
void random_change(tenticle* tent);
void init_colors();
vec2 dir2vec(int dir);
void update_joints(tenticle* tent);

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
	//set up generic
	signal(SIGINT, killer);
   srand(time(NULL)); //set random seed

   //set up ncursees
	initscr();
   noecho();
   cbreak();
   nodelay(stdscr, 1);
   curs_set(FALSE);
	init_colors();

	//get term dimensions
   getmaxyx(stdscr, maxy, maxx);
	
   tenticle tenticles[16]; 

   //create tenticles
	for(int d =0; d < 8; d++){
		tenticle tent = gen_tenticle(maxx/2,maxy/2);
		tent.color = random_number(1,99);
		
		int dir = d%8;
		tent.direction = dir2vec(dir);

		tent.position.x += tent.direction.x;
		tent.position.y += tent.direction.y;

		if(d>=8){
			dir+=random_number(0,1)*4 -2;
			dir = (dir+8)%8;
			tent.direction = dir2vec(dir);
		}	

		render_tenticle(tent);
		tenticles[d]=tent;
	}

	//refresh();
	//usleep(1000000);
	erase();
	
	for(int i=0; i<8; i++){
		for(int j=0; j<tenticles[i].length; j++){
			tenticles[i].joints[j]=1;
		}
		render_tenticle(tenticles[i]);
	}
	refresh();
	usleep(1000000);

	for(int j=0; j<16; j++){
		erase();
		for(int i=0; i<8; i++){
			tenticles[i].joints[j]=0;
			render_tenticle(tenticles[i]);
		}
		refresh();
		if(j!=15){usleep(200000-j*13000);}
	}

	for(int j=15; j>=0; j--){
		erase();
		for(int i=0; i<8; i++){
			tenticles[i].joints[j]=-1;
			render_tenticle(tenticles[i]);
		}
		refresh();
		usleep(200000-j*13000);
	}

	usleep(1000000);

	//animate tenticles	
	while(run){
		erase();

		int a = random_number(0,7);
		int b = random_number(0,1)*2 - 1;

		tenticles[a].delta=b;

		for(int i =0; i<8 ; i++){
			update_joints(&tenticles[i]);
			render_tenticle(tenticles[i]);
		}
		refresh();
		usleep(100000);

		if(random_number(0,100)==20){
			for(int j=0; j<16; j++){
					erase();
				for(int i=0; i<8; i++){
					tenticles[i].joints[j]=0;
					render_tenticle(tenticles[i]);
				}
				refresh();
				usleep(200000-j*10000);
			}

			for(int j=15; j>=0; j--){
				erase();
				for(int i=0; i<8; i++){
					tenticles[i].joints[j]=1;
					render_tenticle(tenticles[i]);
				}
				refresh();
				usleep(200000-j*10000);
			}
		}
	}
	
	endwin();

	/*for(int n=0 ; n<tent.length ; n++){
		printf("point:%d x:%d y:%d t:%d l:%d", n, tent.points[n].x, tent.points[n].y, tent.joints[n], tent.segments[n])
	}*/

}

vec2 dir2vec(int dir){
		assert(dir<8);
		assert(dir>=0);
		vec2 direction;
		switch(dir){
			//up
			case 0:
				direction.x = 0;
				direction.y = 1;
				break;		
			//up right
			case 1:
				direction.x = 1;
				direction.y = 1;
				break;
			//right
			case 2:
				direction.x = 1;
				direction.y = 0;
				break;
			//down right
			case 3:
				direction.x = 1;
				direction.y = -1;
				break;
			//down
			case 4:
				direction.x = 0;
				direction.y = -1;
				break;
			//down left
			case 5:
				direction.x = -1;
				direction.y = -1;
				break;
			//left
			case 6:
				direction.x = -1;
				direction.y = 0;
				break;
			//up left
			case 7:
				direction.x = -1;
				direction.y = 1;
				break;
		}

		return direction;
}

void random_change(tenticle* tent){
	int a = random_number(0,7);//TODO not always 7
	int b = random_number(0,7);
	if((*tent).joints[b]==0){
		(*tent).joints[b] = random_number(0,1)*2 -1;
	}else{
		(*tent).joints[b]*=-1;
		/*for(int i = a+1 ; i<8 ; i++){
			//movement easing
			if((*tent).joints[b]==(*tent).joints[i]){
				(*tent).joints[i]*=-1;
				break;
			}
		}*/
	}
		
}

tenticle gen_tenticle(int xoffset, int yoffset){
	tenticle tent;
	tent.length = 16;
	tent.position.x=xoffset;
	tent.position.y=yoffset;
	tent.direction.x=0;
	tent.direction.y=1;
	tent.inflection=4;
	tent.first_curl=1;
	for(int n=0; n<tent.length ; n++){
		tent.joints[n] = random_number(0,1)*2 - 1;
		tent.segments[n]=tent.length-n;
		//tent.segments[n]=4;
	}
	tent.segments[0]=3;
	tent.segments[tent.length-1]=3;
	return tent;
}

void update_joints(tenticle* tent){

	int zero = -1;

	(*tent).inflection+=(*tent).delta;
	if( (*tent).inflection > (*tent).length){
		(*tent).inflection = 0;
		(*tent).first_curl *= -1;
	}else if ((*tent).inflection < 0){
		(*tent).inflection = (*tent).length;
		(*tent).first_curl *= -1;
	}else if((*tent).delta>0){
		zero = (*tent).inflection-1;
	}else if((*tent).delta < 0){
		zero = (*tent).inflection;
	}

	for(int j = 0 ; j < (*tent).length ; j++){
		if(j<(*tent).inflection){
			(*tent).joints[j]= (*tent).first_curl;
		}else{
			(*tent).joints[j]= -(*tent).first_curl;	
		}
	}

	if(zero != -1){
		(*tent).joints[zero] = 0;
	}
	(*tent).delta = 0;
}

void render_tenticle(tenticle tent){
	int n=0;
	tent.points[n].x = tent.position.x;
	tent.points[n].y = tent.position.y;
		
	vec2 dir;//current tenticle segment direction in rendering
	dir.x = tent.direction.x;
	dir.y = tent.direction.y;

	while(n<tent.length){
		//sets next point
		tent.points[n+1].x=tent.points[n].x+ tent.segments[n]*dir.x;
		tent.points[n+1].y=tent.points[n].y+ tent.segments[n]*dir.y;

		//the following will turn the dir 45 degrees clockwise if tent.joints is 1
		//and 45 degrees counterclockwise if tent.joints is -1, and leave  the dir 
		//as is if tent.joints is 0. NOTE: dir decides which direction the next 
		//segment of the tenticle will go.
		int turn = tent.joints[n];
		if(turn==0){
		//do nothing
		}else if(dir.y==0){
			dir.y = -1 * turn * dir.x;
		}else if(dir.x==0){
			dir.x = turn * dir.y;
		}else if(dir.x && dir.y){
			if(dir.x * dir.y * turn > 0){
				dir.y=0;
			}else{
				dir.x=0;
			}
		}

		n++;
	}


	attron(COLOR_PAIR(tent.color));
	draw_path(tent.points, tent.length+1);
}

void draw_path(vec2* points, int num_points){
	int cy = points[0].y;
	int cx = points[0].x;			

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
			ty = points[t].y;
			tx = points[t].x;
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



