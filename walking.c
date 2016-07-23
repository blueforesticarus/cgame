#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ncurses.h>
#include <signal.h>
#include <time.h>

#include "parser.c"

#define DELAY 50000

#define WASD
#ifdef WASD
	#define KEY_UP 'w'
	#define KEY_DOWN 's'
	#define KEY_LEFT 'a'
	#define KEY_RIGHT 'd'
#endif

void rlbuff(float delta_t); // Rate Limit the buffer
float get_timespec_diff_ms(struct timespec start, struct timespec end);

int run=1, ice_rink = 0;
void killer(int dummy){
 run=0;
}

vec2 pos;
int maxx, maxy;
struct timespec start_frame, end_frame;
float delta_t = 0.0f;
int show_framerate = 0;

int main(int argc, char *argv[]) {
	signal(SIGINT, killer);

    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, 1);
    curs_set(FALSE);
	getmaxyx(stdscr, maxy, maxx);

    start_color();			/* Start color 			*/
	
	pos.x = 20;
	pos.y = 20;
	
	rstring buf = read_file("demoland.ated");
	ated landscape = parse(buf);
	
	for(int i; i<256;i++){
		init_color(i, (landscape.colors[i].r*1000)/256, (landscape.colors[i].g*1000)/256, (landscape.colors[i].b*1000)/256);
		init_pair(i, 0,i);
	}

	//init_pair(landscape.colormap['B'],landscape.colormap['1'],landscape.colormap['B']);//TODO do properly
	init_pair(landscape.colormap['1'],landscape.colormap['B'],landscape.colormap['B']);//this is hacky

	vec2 newpos;
	while(run){
		clock_gettime(CLOCK_REALTIME, &start_frame);
		erase();

		for(int c = 0 ; c<landscape.columns; c++){
			for(int r = 0 ; r<landscape.rows; r++){
				attron(COLOR_PAIR( landscape.colormap[landscape.states[0].colormap[c+r*landscape.columns]] ));
				mvaddch(r,c,landscape.states[0].symbols[c+r*landscape.columns]);	
			}
		}
		
		//attron(A_BOLD | A_DIM);
		if(landscape.states[0].hitmap[newpos.x+(pos.y-1)*landscape.columns] != 'B' ){
			if(landscape.states[0].hitmap[newpos.x+(pos.y-1)*landscape.columns] != 'H' && landscape.states[0].hitmap[pos.x+(pos.y-1)*landscape.columns] != 'n' ){
				attron(COLOR_PAIR( landscape.colormap[landscape.states[0].colormap[pos.x+(pos.y-1)*landscape.columns]] ));
				mvaddch(pos.y-1,pos.x,'o');
			}
		}
		if(landscape.states[0].hitmap[newpos.x+(pos.y)*landscape.columns] != 'B' ){
			if(landscape.states[0].hitmap[pos.x+(pos.y)*landscape.columns] != 'T' ){
				attron(COLOR_PAIR( landscape.colormap[landscape.states[0].colormap[pos.x+pos.y*landscape.columns]] ));
				mvaddch(pos.y,pos.x,'|');
			}	
		}
		//attroff(A_BOLD);

		if (show_framerate) mvprintw(1, 0, "%f", (delta_t));
        if (ice_rink) mvprintw(0, 0, "Weeeee!");
		refresh();
		usleep(DELAY);

		newpos.x = pos.x;
		newpos.y = pos.y;
		char key = getch();
		if(key == KEY_UP){
			newpos.y-=1;
		}else if(key == KEY_DOWN){
			newpos.y+=1;
		}else if(key == KEY_RIGHT){
			newpos.x+=1;
		}else if(key == KEY_LEFT){
			newpos.x-=1;
		}else if(key == 'f'){
			show_framerate += 1;
			show_framerate %= 2;
		}else if(key == 'q'){
            run = 0;
        }

		if(landscape.states[0].hitmap[newpos.x+newpos.y*landscape.columns] == 'N' || landscape.states[0].hitmap[newpos.x+newpos.y*landscape.columns] == 'n'){
			//todo play sound
		}else{
			pos.x=newpos.x;
			pos.y=newpos.y;
		}

        if (landscape.states[0].hitmap[pos.x+pos.y*landscape.columns] == 'I') ice_rink = 1;
        else ice_rink = 0;
	
    clock_gettime(CLOCK_REALTIME, &end_frame);
	delta_t = get_timespec_diff_ms(start_frame, end_frame);
	if (ice_rink == 0) rlbuff(delta_t);

	}
	endwin();	
}

void rlbuff(float delta_t){ // Rate limit the buffer
	int num_keypress = (int) (delta_t * 15);
	while (num_keypress--) getch();
}

float get_timespec_diff_ms(struct timespec start, struct timespec end){
    return ((float)(end.tv_sec - start.tv_sec) * 1000.0) + ((float)(end.tv_nsec - start.tv_nsec) / 1000000.0);
}
