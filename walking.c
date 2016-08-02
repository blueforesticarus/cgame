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
        #undef KEY_UP
        #undef KEY_DOWN
	#undef KEY_LEFT
        #undef KEY_RIGHT
	#define KEY_UP 'w'
	#define KEY_DOWN 's'
	#define KEY_LEFT 'a'
	#define KEY_RIGHT 'd'
	#define KEY_LUP 'r'
	#define KEY_LDOWN 'f'
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
int show_hitmap = 0;
int ladder = 0;
vec2 frame_offset;

int main() {
	signal(SIGINT, killer);

    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, 1);
    curs_set(FALSE);
	getmaxyx(stdscr, maxy, maxx);

    start_color();			/* Start color 			*/
	
	pos.x = 76;
	pos.y = 25;
        frame_offset.x=0;
        frame_offset.y=10;

	rstring buf = read_file("demoland.ated");
	ated land = parse(buf);	
        buf = read_file("house.ated");
        ated house = parse(buf);
        buf = read_file("tower.ated");
        ated tower = parse(buf);
        vec2 last;
	vec2 offset;

        ated landscape = land;
		
	for(int i; i<256;i++){
		init_color(i, (landscape.colors[i].r*1000)/256, (landscape.colors[i].g*1000)/256, (landscape.colors[i].b*1000)/256);
		init_pair(i, 0,i);
	}
        init_color(120,0,0,0);
        init_color(121,1000,1000,1000);
        init_pair(120,120,120);
        init_pair(121,121,120);

	vec2 newpos;
	while(run){
		clock_gettime(CLOCK_REALTIME, &start_frame);
		erase();

		for(int c = 0 ; c<maxx; c++){
			for(int r = 0 ; r<maxy; r++){
				if(c-offset.x >= 0 && c-offset.x < landscape.columns && r-offset.y >= 0 && r-offset.y < landscape.rows ){
					if(! show_hitmap){
						attron(COLOR_PAIR( land.colormap[landscape.states[0].colormap[c-offset.x + (r-offset.y)*landscape.columns]] ));
						mvaddch(r,c,landscape.states[0].symbols[c-offset.x+(r-offset.y)*landscape.columns]);
					}else{
						attron(COLOR_PAIR( land.colormap[landscape.states[0].colormap[c-offset.x + (r-offset.y)*landscape.columns]] ));
						mvaddch(r,c,landscape.states[0].hitmap[c-offset.x+(r-offset.y)*landscape.columns]);	
					}
				}else{
					attron(COLOR_PAIR(120));
					mvaddch(r,c,'X');
				}
			}
		}
		char hitcode = landscape.states[0].hitmap[pos.x-offset.x+(pos.y-offset.y)*landscape.columns];
		char hitcode2 = landscape.states[0].hitmap[pos.x-offset.x+(pos.y-offset.y-1)*landscape.columns];
		//attron(A_BOLD | A_DIM);
		if(hitcode2 != 'B' && (ladder || (hitcode2 != 'l' && hitcode2 != 'L' && hitcode2 !='g') || hitcode == 'u') && !(ladder && hitcode2=='h')){
			if(hitcode2 != 'H' && hitcode2 != 'n' && hitcode2 != 'q'){
				attron(COLOR_PAIR( land.colormap[landscape.states[0].colormap[pos.x-offset.x+(pos.y-1-offset.y)*landscape.columns]] ));
				mvaddch(pos.y-1,pos.x,'o');
			}
		}
		if(hitcode != 'B' && (ladder || (hitcode != 'l' && hitcode != 'L' && hitcode != 'g')) && !(ladder && (hitcode=='h'|| hitcode=='q'))){
			if(hitcode != 'T' ){
				attron(COLOR_PAIR( land.colormap[landscape.states[0].colormap[pos.x-offset.x + (pos.y-offset.y)*landscape.columns]] ));
				mvaddch(pos.y,pos.x,'|');
			}	
		}
		//attroff(A_BOLD);
		if (show_framerate) {
			attron(COLOR_PAIR(121));
			mvprintw(1, 0, "%f", (delta_t));
			mvprintw(2, 0, "X:%d Y:%d", pos.x, pos.y);
			mvprintw(3, 0, "oX:%d oY:%d", offset.x, offset.y); 
			mvprintw(4, 0, "hm-H:%c hm-B:%c", hitcode2, hitcode);
                        mvprintw(5, 0,"ladder: %d",ladder);
                        mvprintw(6,0, "color: %d cid: %c(%d)",
				land.colormap[landscape.states[0].colormap[pos.x-offset.x + (pos.y-offset.y)*landscape.columns]],
				landscape.states[0].colormap[pos.x-offset.x + (pos.y-offset.y)*landscape.columns],
				(int)landscape.states[0].colormap[pos.x-offset.x + (pos.y-offset.y)*landscape.columns]);
		}
        if (ice_rink) mvprintw(0, 0, "Weeeee!");
		refresh();
		usleep(DELAY);

		newpos.x = pos.x;
		newpos.y = pos.y;
		char key = getch();
		//these next lines are a bit hacky
		if(key == KEY_UP && (ladder || hitcode=='u')){
			key=KEY_LUP;
		}
		if(key == KEY_DOWN && (ladder || hitcode=='d')){
			key=KEY_LDOWN;
		}

		if(key == KEY_UP){
			newpos.y-=1;
		}else if(key == KEY_LUP && (ladder || hitcode=='u') && hitcode != 'd'){
			ladder = true;
			newpos.y-=1;
		}else if(key == KEY_LDOWN && (ladder || hitcode=='d') && hitcode != 'u'){
			ladder = true;
			newpos.y+=1;
		}else if(key == KEY_DOWN){
			newpos.y+=1;
		}else if(key == KEY_RIGHT){
			newpos.x+=1;
		}else if(key == KEY_LEFT){
			newpos.x-=1;
		}else if(key == 'i'){
			show_framerate += 1;
			show_framerate %= 2;
                }else if(key == 'h'){
                        show_hitmap = ! show_hitmap;
		}else if(key == 'q'){
            		run = 0;
        	}
		char hitcodeold = hitcode;
                hitcode = landscape.states[0].hitmap[newpos.x-offset.x+(newpos.y-offset.y)*landscape.columns];
		if(hitcode == 'N' || hitcode == 'n' || 
			(ladder && (hitcodeold == 'h' || hitcodeold == 'l' || hitcodeold == 'L' || hitcodeold == 'q') && key != KEY_LDOWN && key != KEY_LUP) || 
			(key==KEY_DOWN && (hitcode == 'D'|| hitcode == 'g')) || (key == KEY_UP && (hitcodeold == 'D' || hitcodeold == 'g')) || 
			(!ladder && hitcode=='L') || (hitcodeold == 'u' && key == KEY_UP)//comment
		){
			//todo play sound
		}else if(hitcode == 'W'){
                        if(strcmp(land.name,landscape.name)==0){ 
				vec2 entrance = find(house,'W');
				if(entrance.x != -1){					
					landscape = house;         
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = pos.x - entrance.x;
                                	offset.y = pos.y - entrance.y;
					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;
				}
                        }else{
  				vec2 entrance = find(land,'W');
				if(entrance.x != -1){					
					landscape = land;         
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = frame_offset.x;
                                	offset.y = frame_offset.y;
					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;
				}
			}
                }else if(hitcode == 'a'){
                        if(strcmp(land.name,landscape.name)==0){
				vec2 entrance = find(tower,'a');
				if(entrance.x != -1){					
					landscape = tower;         
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = pos.x - entrance.x;
                                	offset.y = pos.y - entrance.y;					
					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;
				}
                        }else{
  				vec2 entrance = find(land,'a');
				if(entrance.x != -1){					
					landscape = land;         
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = 0;
                                	offset.y = 0;
					
					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;
				
				}
			}
                }else if(hitcode == 'b'){
                        if(strcmp(land.name,landscape.name)==0){ 
				vec2 entrance = find(tower,'b');
				if(entrance.x != -1){					
					landscape = tower;         
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = pos.x - entrance.x;
                                	offset.y = pos.y - entrance.y;
					
					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;
					
				}
                        }else{
  				vec2 entrance = find(land,'b');
				if(entrance.x != -1){					
					landscape = land;         
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = 0;
                                	offset.y = 0;
					
					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;
				
				}
			}
                }else if(hitcode == 'c'){
                        if(strcmp(land.name,landscape.name)==0){
				vec2 entrance = find(tower,'c');
				if(entrance.x != -1 && key == KEY_DOWN){		
					landscape = tower;
					ladder = true; 
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = pos.x - entrance.x;
                                	offset.y = pos.y - entrance.y;
				
					
					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;
				}
                        }else{
  				vec2 entrance = find(land,'c');
				if(entrance.x != -1){					
					landscape = land;         
					last.x = pos.x;
					last.y = pos.y;
                                	offset.x = 0;
                                	offset.y = 0;

					pos.x= entrance.x + offset.x + newpos.x-pos.x;
					pos.y= entrance.y + offset.y + newpos.y-pos.y;

				}
			}
                }else{
			pos.x=newpos.x;
			pos.y=newpos.y;
			if(key == KEY_UP || key == KEY_DOWN || key == KEY_RIGHT || key == KEY_LEFT){
				ladder = false;
			}
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
