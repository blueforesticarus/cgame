//this is to read from .ated [ascii text entity definition] files
//files have a few parts
//part one: 
//	entity name:
//	entity identifier:
//	entity description:
//part two:
//	number of colors:
//	color definitions:
//part three: states
//	number of states:
//	lines:
//	columns: 
//	center:	
//part four:
//	ascii data for each state
//	color data for each state
//	hitmapping for each state
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <ncurses.h>

#define SEGMENT_BUFFER_LEN 1000
#define SEGMENT_DELIMIT '='

void printfn(const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	putchar('\n');
	va_end(args);
}

char* substring(char* src, int len){
	char* sub =	malloc(len*sizeof(char));
	for(int i=0; i<len; i++){
		sub[i]= src[i];	
	}
	return sub;
}

typedef struct{
    int x;
    int y;
} vec2;

typedef struct {
	//note that none of the following are null terminated
	//as NULL (zero byte) may be a valid character
	char* symbols;	//characters
	char* hitmap;	//hitmap
	char* colormap;//colors

	vec2 center;	//center of block for positioning
	char* name;	//name of state
}ated_state;


typedef struct {
	short r;
	short g;
	short b;
}color;

typedef struct {
	char* display;		//in game display name
	char* description;	//description (for dev/debug)
	char* name;		//in code name
	short id;		//unique identifier
	
	short columns;		//number of columns
	short rows;		//number of rows
	int block;		//convienience variable for rows*columns		
		
	ated_state* states;	//pointer to array of states
	short num_states;		//number of states

	color colors[255];	//array of colors used by entity
	short colormap[255];		//colormap
} ated;

typedef struct {
	char* text; //not null terminated
	int length; //number of chars
	int position; //current position in buffer

} rstring;

rstring read_file(char* filename);
ated parse(rstring text);
void dealloc_ated(ated object);
void test();

int main(int argc, char *argv[]) {
	test();
}

void test(){
	printf("running test\n");

	rstring buf = read_file("parsetest.ated");

	ated myentitydef = parse(buf);
	printfn("Name: %s",myentitydef.name);
	usleep(500000);

	initscr();
   noecho();
   cbreak();
   nodelay(stdscr, 1);
   curs_set(FALSE);

	for(int s = 0 ; s<myentitydef.num_states; s++){
		for(int c = 0 ; c<myentitydef.columns; c++){
			for(int r = 0 ; r<myentitydef.rows; r++){
				mvaddch(r,c,myentitydef.states[s].symbols[c+r*myentitydef.columns]);
			}
		}
		refresh();
		sleep(1);
	}

	endwin();	
	dealloc_ated(myentitydef); 
}


void dealloc_ated(ated object){
	//TODO
}

//return string from text file, null terminated
//puts length of file into passed pointer
rstring read_file(char* filename){
	rstring text;
	text.position = 0;

	FILE * f = fopen (filename, "rb");

	if (f)
	{
		fseek (f, 0, SEEK_END);
		//TODO: shouldnt it be one more than this
		text.length = ftell (f);
		fseek (f, 0, SEEK_SET);
		//note: must free text later
		text.text = malloc (text.length);
		if (text.text){
			fread (text.text, 1, text.length, f);
		}
		fclose (f);
	}
	printf(text.text);
	return text;
}

char get_byte(rstring* buff){
	char c = (*buff).text[(*buff).position];
	//printf("%d \n",(*buff).position);	
	(*buff).position++;
	
	if((*buff).position==(*buff).length){
		//loop back
		(*buff).position=0;
	}

	return c;

	//TODO get stuff below working
	if(c==92){
		//handle backslash escaping
		c = (*buff).text[(*buff).position];
		//TODO add support for \n \t etc
		(*buff).position++;
		if((*buff).position==(*buff).length){
			//loop back
			(*buff).position=0;
		}
	}else if(c==10){
		//ignore newlines unless escaped
		c = get_byte(buff);
	}
	return c;
}

ated parse(rstring text){
	printf("testing\n");
	char buffer[SEGMENT_BUFFER_LEN];//create a buffer for parts of the file
	int len=0;			//current length of string in buffer

	static ated entityDef;	//object to contain parsed file data
	int segment = 0;//which segment of the file the scanner is in 

	int i = 0;
	int header_flg = 1; //boolean flag 
	//TODO what if header never becomes true (currently causes seg faulkt)
	for (; header_flg; i++){
		
		char character = get_byte(&text);

		//printf("char: %c %d, %d\n",character, len, i);
		
		//TODO handle file to short
		if(i>=SEGMENT_BUFFER_LEN){
			printf("ERROR: Parser Buffer Overflow.");
			break;//TODO error handle		
		}
		else if(character==SEGMENT_DELIMIT){
			buffer[len]=0;//null terminate the buffer

			printf("Header: ");
			fwrite(buffer, 1, len, stdout);
			printf("\n\n");

			switch(segment) {
				case 0 :
					//everything prior to first delimiter is a comment	
					break;				
				case 1:
					//name
					//TODO MUST FIX, this may result in undefined behavior, I think that I may have to malloc an extra byte for the null terminator
					entityDef.name = malloc(len*sizeof(char));
					strncpy(entityDef.name, buffer, len);
					break;				
				case 2:
					//display
					entityDef.display = malloc(len*sizeof(char));
					strncpy(entityDef.display, buffer, len);
					break;				
				case 3:
					//description
					entityDef.description = malloc(len*sizeof(char));
					strncpy(entityDef.description, buffer, len);
					break;				
				case 4:
					//id
					entityDef.id = strtol(buffer, NULL, 10);
					break;				
				case 5:
					//columns
					entityDef.columns = strtol(buffer, NULL, 10);
					break;				
				case 6:
					//rows
					entityDef.rows = strtol(buffer, NULL, 10);
					entityDef.block = entityDef.rows * entityDef.columns;
					break;
				case 7:
					//states
					entityDef.num_states = strtol(buffer, NULL, 10);
					entityDef.states = malloc(entityDef.num_states*sizeof(ated_state));
					break;		
				case 8:
					//end of header data, break out of for loop
					header_flg = 0;
			}
			segment++;
			len=0;
		}else{
			buffer[len]=character;
			len++;
		}
	}

	text.position = i;//start of data segment of the config (symbols, colors, hitbox)
	printfn("%d, %d, %d",entityDef.rows, entityDef.columns, entityDef.block);
	//printfn("Data:%s", data); doesn't work with buffer model

	for(int n=0;n<entityDef.num_states;n++){
		//fwrite(data + n*entityDef.block,1,entityDef.block,stdout); putchar('\n');
		//entityDef.states[n].symbols = substring(data + n*entityDef.block, entityDef.block);
		
		entityDef.states[n].symbols = malloc(entityDef.block*sizeof(char));
		for(int i=0; i<entityDef.block;i++){

			entityDef.states[n].symbols[i]=get_byte(&text);	
		}
	}

	return entityDef;
}



