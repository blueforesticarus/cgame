#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ncurses.h>
#include <signal.h>

#include "parser.c"

#define DELAY 50000

#define WASD
#ifdef WASD
	#define KEY_UP 'w'
	#define KEY_DOWN 's'
	#define KEY_LEFT 'a'
	#define KEY_RIGHT 'd'
#endif

int run=1;
void killer(int dummy){
 run=0;
}

vec2 pos;
int maxx, maxy;

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


	char press;
	vec2 newpos;
	while(run){
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
		}
		press=key;

		if(landscape.states[0].hitmap[newpos.x+newpos.y*landscape.columns] == 'N' || landscape.states[0].hitmap[newpos.x+newpos.y*landscape.columns] == 'n'){
			//todo play sound
		}else{
			pos.x=newpos.x;
			pos.y=newpos.y;
		}


	}
	endwin();	
}

