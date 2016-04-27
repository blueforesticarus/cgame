// demo.c
#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>

#include "sound.c"

#ifdef KEY_ENTER
	#undef KEY_ENTER
#endif
#define KEY_ENTER 10

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define SAMPLE_RATE 44100
#define STARCOUNT 1000

void killer(int dummy);
char *randstring(int length);
void genstars(int num);
int random_number(int min_num, int max_num);
void handle_winch(int sig);
char *getstring(int length);

pthread_t mt; // music thread.
pthread_t st; // sound fx thread

static volatile int run = 1;
int maxx, maxy;

struct vec2{
    int x;
    int y;
};

struct vec2 stars[STARCOUNT];

const adata_t DEFAULT_THREAD_STATUS = { .volume = 1.0, .thread_complete = 0};

char user_in;

int main(int argc, char *argv[]) {

	signal(SIGINT, killer);

    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, 1);
    curs_set(FALSE);
    
    signal(SIGWINCH, handle_winch);
    
    start_color();			/* Start color 			*/
    
	adata_t mthread_status = DEFAULT_THREAD_STATUS;
	adata_t fxthread_status = DEFAULT_THREAD_STATUS;
	mthread_status.filepath = "death_and_maiden.ogg";
    int ret = pthread_create(&mt, NULL, (void *) &handle_sound, &mthread_status);
    if (ret)
    {
        printf("Failed to start thread: %d\n", ret);
        exit(ret);
    }

    getmaxyx(stdscr, maxy, maxx);
    
    int n,m;
    int TOTALCOLORS=0;
    for(n=0;n<8;n++){
     for(m=0;m<8;m++){
       if(n!=m){
        TOTALCOLORS+=1;
        init_pair(TOTALCOLORS, n,m);
       }
     }
    }
    
    genstars(STARCOUNT);
    
    int titleWidth = 44;
    int titleHeight = 9;
    int title[] = {
        1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 2,1, 3,1, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 5,6, 6,6, 7,1, 7,2, 7,3, 7,4, 7,5, 7,6,
        1,10, 1,15, 2,10, 2,15, 3,10, 3,15, 4,10, 4,11, 4,12, 4,13, 4,14, 4,15, 5,10, 5,15, 6,10, 6,15, 7,10, 7,15,
        1,19, 1,20, 1,21, 1,22, 1,23, 1,24, 2,19, 2,24, 3,19 ,3,24, 4,19, 4,20, 4,21, 4,22, 4,23, 4,24, 5,19, 5,24, 6,19, 6,24, 7,19, 7,24,
        1,28, 1,29, 1,30, 1,31, 1,32, 2,28, 2,32, 2,33, 3,28, 3,33, 4,28, 4,33, 5,28, 5,33, 6,28, 6,32, 6,33, 7,28, 7,29, 7,30, 7,31, 7,32,
        1,37, 1,38, 1,39, 1,40, 1,41, 1,42, 2,37, 3,37, 4,37, 4,38, 4,39, 4,40, 4,41, 4,42, 5,37, 6,37, 7,37, 7,38, 7,39, 7,40, 7,41, 7,42	
    };
    
    int titleLength = NELEMS(title);
    init_color(9, 289, 500, 250);
    init_color(10, 200, 300, 200);
    init_pair(101, 9, 10);
    init_pair(102, 10, 9);
    int i = 0; 
    int len = 0;
    int pair;
    int max = 1;
    int re = 0;
    float rem = 0;
    int length = 1;
    int stage = 0;//0; //this variable keeps track of which part of the sequence we are in
    int progress = 0;
    int progress2 = 0;
    int progress3 = 0;
    int line;
    int col;
    bool starState = false;
    int offsetx;
    int offsety;
    
    int sleeper =6000;
    
    if(argc==2){
    int counter = 0;
    int color = 0;
    for(col=0; col<=maxx*(maxy/16 + 1); col+=1){
        for(line=0; line<16; line+=1){
            init_color(1+counter, color/256 *1000 /16, (color/16)%16 *1000 /16, color%16 *1000 /16);
            //init_color(200+counter,500,500,500);
            init_pair(1+counter, 1+counter,1+counter); 
            mvaddch(line + 16*(col/maxx),col%maxx,(char)32 | COLOR_PAIR(1+counter));
            //attroff(COLOR_PAIR(200+counter)); 
            counter++;
            color++;
            if(counter>255){
                counter=0;
            }
            if(color>=16*16*16){
                color=0;
            }
        }
		if(!run){
			break;
		}
        refresh();
        usleep(10000);
     }
     getch();
     endwin();
     exit(0);
    }
    
    while(run){
      if(len < 1000 ) len+=2;
      else if(stage==0){
       stage=1;
       sleeper=8000;
      }
      pair = random_number(1,TOTALCOLORS);
      attron(COLOR_PAIR(pair));
      max = len;
      length = random_number(0,max);
      length = length*length/max;
      char* text = randstring(length);
      //char* text = randstring(10000);
      mvaddstr(random_number(0,maxy), random_number(0,10), text);
       attroff(COLOR_PAIR(pair));
       
       if(re==0){
    
          if(stage>=1){
              for(line=0; line<=maxy; line+=1){
                 for(col=0; col<=progress; col+=1){
                  if(line%2==0){
                     mvaddch(line,maxx-col,(char)219);	
                  }else{
                     mvaddch(line,col,(char)219);	
                  }
                 }
              }
    
             if(progress>=maxx || stage>1){
                  progress=maxx;
                  if(stage==1){
                      stage=2;
                      sleeper=10000;
                  }
              }else{
                progress+= ((progress * 8) / maxx ) + 1;
                progress+=3;
              }
          }
    
          if(stage>=2){
            for(col=0; col<=maxx; col+=1){
              for(line=0; line<=maxy; line+=1){
                  if( abs(col - maxx/2) < progress2){
                     mvaddch(line,maxx-col,(char)32);	
                  }
              }
            }
                
            init_color(8, progress2*2500/maxx , progress2*2500/maxx, progress2*2500/maxx);
            init_pair(100, 8, COLOR_BLACK);
            attron(COLOR_PAIR(100));
    
            for(i = 0 ; i<120 ; i++){			
                if( abs(stars[i].x - maxx/2) + 1 >= progress2 ) continue;
                //if(starState != (i%2==0)){
                if(random_number(0,100)>75){
                     mvaddch(stars[i].y, stars[i].x, (char)32);	
                }else{
                     mvaddch(stars[i].y, stars[i].x, (char)46);	
                }
                
            }
            starState = !starState;
            attroff(COLOR_PAIR(100));
    
              if(progress2>=maxx/3 || stage>2){
                  progress2=maxx/3;
                  if(stage==2){
                    stage=3;
                    sleeper=12000;
                  }
              }else{
                //progress+= (progress * 8) / maxx;
                progress2+=1;
              }
          }
    
          if(stage>=3){
    
            offsetx= (maxx-titleWidth)/2;
            offsety= (maxy-titleHeight)/2;
    
            attron(COLOR_PAIR(101));
            for(i=0;i<titleLength;i+=2){
                mvaddch(title[i]+offsety, title[i+1]+offsetx,(char)32);	
            }
            attroff(COLOR_PAIR(101));
    
            attron(COLOR_PAIR(102));
            for(i=0;i<titleLength;i+=2){
                mvaddch(title[i]+offsety+1, title[i+1]+offsetx-1,(char)32);	
            }
            attroff(COLOR_PAIR(102));
    
            if(progress3<25){
                progress3 += 1; 
            }else{
                //attron(A_BLINK); for some reason blinking doesn't work in rxvt
                mvaddstr(offsety+titleHeight+2, (maxx/2) - 6, "[Press Enter]");
                //attroff(A_BLINK);
            }
          }
          user_in = getch();
		  if (user_in == 'q' || user_in == 'Q') run = 0;
          
		  if (user_in == KEY_ENTER && progress3 >= 25)
          {
		  	fxthread_status.filepath = "enter.ogg";
           	int ret = pthread_create(&st, NULL, (void *) &handle_sound, &fxthread_status);
            if (ret)
            {
                printf("Failed to start thread: %d\n", ret);
                exit(ret);
            }	
          }
		  
		  if (user_in == '-') mthread_status.volume -= 0.05;
		  
		  if (user_in == '+' || user_in == '=') mthread_status.volume += 0.5;
          refresh();
          re=(int)rem;
          if(rem < 5) rem+=.1;
           
       }else{
           re--;
       }
       free(text);

       usleep(sleeper); 
    
     }
    
    
     endwin(); // Restore normal terminal behavior
     Pa_Terminate();
}

void genstars(int num){
    int i;
    for(i=0;i<num;i++){
        stars[i].x=random_number(maxx/6 + 2, 5*maxx/6 - 1);
        stars[i].y=random_number(0,maxy);
    }
}


void killer(int dummy){
 run=0;
}

void handle_winch(int sig)
{
    endwin();
    // Needs to be called after an endwin() so ncurses will initialize
    // itself with the new terminal dimensions.
    refresh();
    clear();

    getmaxyx(stdscr, maxy, maxx);
    genstars(STARCOUNT);
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
    //srand(time(NULL));
    result = (rand()%(hi_num-low_num))+low_num;
    return result;
}

char *getstring(int length){
 char *mystring = "";
 for(;length>0;length--){
  mystring = malloc(strlen(mystring)+1);
  char pop = getchar();
  strcat(mystring, &pop);
 }

 return mystring;
}

char *randstring(int length) {    
    static int mySeed = 25011984;
    char *string = "+=O|";
    size_t stringLen = strlen(string);        
    char *randomString = NULL;

    srand(time(NULL) * length + ++mySeed);

    if (length < 1) {
        length = 1;
    }

    randomString = malloc(sizeof(char) * (length +1));

    if (randomString) {
        short key = 0;

    int n;
        for (n = 0;n < length;n++) {            
            key = rand() % stringLen;          
            randomString[n] = string[key];
        }

        randomString[length] = '\0';

        return randomString;        
    }
    else {
        printf("No memory");
        exit(1);
    }
}

