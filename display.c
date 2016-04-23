// demo.c
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

static volatile int run = 1;

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
  strcat(mystring, getch());
 }
}

char *randstring(int length) {    
    static int mySeed = 25011984;
    char *string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/";
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

 int len = 0;
 int pair;
 while(run){
   len++;
   pair = random_number(1,TOTALCOLORS);
   attron(COLOR_PAIR(pair));
   mvprintw(0, 0, strcopy(random_number(0,len/100)));
   attroff(COLOR_PAIR(pair));
   refresh(); 
 }

 endwin(); // Restore normal terminal behavior
}

