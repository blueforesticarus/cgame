// demo.c
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

static volatile int run = 1;

struct point{
	int x;
	int y;
};

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

int main(int argc, char *argv[]) {
 signal(SIGINT, killer);

 initscr();
 noecho();
 curs_set(FALSE);

 start_color();			/* Start color 			*/

 int maxx, maxy;
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

struct point stars[50];
int i;
for(i=0;i<50;i++){
	stars[i].x=random_number(maxx/6 + 1, 5*maxx/6 - 1);
	stars[i].y=random_number(0,maxy);
}


int titleWidth = 40;
int titleHeight = 9;
int title[] = {
	1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 2,1, 3,1, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 5,6, 6,6, 7,1, 7,2, 7,3, 7,4, 7,5, 7,6,
	1,9 1,14 2,9 2,14 3,9 3,14, 4,9 4,10 4,11 4,12 4,13, 4,14, 5,9, 5,14, 6,9, 6,14, 7,9, 7,14,
    1,17, 1,18, 1,19, 1,20, 1,21, 1,22, 2,17, 2,22, 3,17 ,3,22, 4,17, 4,18, 4,19, 4,20, 4,21, 4,22, 5,17, 5,22, 6,17, 6,22, 7,17, 7,22,
	1,25, 1,26, 1,27, 1,28, 1,29, 2,25, 2,29, 2,30, 3,25, 3,30, 4,25, 4,30, 5,25, 5,30, 6,25, 6,29, 6,30, 7,25, 7,26, 7,27, 7,28, 7,29,
	1,33, 1,34, 1,35, 1,36, 1,37, 1,38, 2,33, 3,33, 4,33, 4,34, 4,35, 4,36, 4,37, 4,38, 5,33, 6,33, 7,33, 7,34, 7,35, 7,36, 7,37, 7,38	
};

 int len = 0;
 int pair;
 int max = 1;
 int re = 0;
 float rem = 0;
 int length = 1;
 int stage = 0;
 int progress = 0;
 int progress2 = 0;
 int line;
 int col;
 bool starState = false;


 while(run){
   if(len < 1000 ) len+=2;
   else if(stage==0){
	stage=1;
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

		  if(progress>=maxx){
			  progress=maxx;
			  stage=2;
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
			
		init_color(8, progress2*3000/maxx , progress2*3000/maxx, progress2*3000/maxx);
		init_pair(100, 8, COLOR_BLACK);
		attron(COLOR_PAIR(100));

		for(i = 0 ; i<50 ; i++){			
			if( abs(stars[i].x - maxx/2) >= progress2 ) continue;
			if(starState != (i%2==0)){
			     mvaddch(stars[i].y, stars[i].x, (char)32);	
			}else{
			     mvaddch(stars[i].y, stars[i].x, (char)43);	
			}
			
		}
		starState = !starState;
		attroff(COLOR_PAIR(100));

		  if(progress2>=maxx/3){
			  progress2=maxx/3;
			  stage=3;
		  }else{
			//progress+= (progress * 8) / maxx;
			progress2+=1;
		  }
	  }

	  if(stage>=3){
		attron(COLOR_PAIR(100));
		
	  }
	   refresh();
	   re=(int)rem;
	   if(rem < 5) rem+=.1;
   }else{
       re--;
   }
   free(text);
   

	 

   usleep(10000); 


 }
 endwin(); // Restore normal terminal behavior
}

