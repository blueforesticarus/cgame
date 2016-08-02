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
#ifndef PARSER_C
#define PARSER_C
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <ncurses.h>

#define SEGMENT_BUFFER_LEN 1000
#define SEGMENT_DELIMIT '='

char* concat(char *s1, char *s2);

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

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
	char colormap[256];		//colormap, each char goes to a color
} ated;

typedef struct {
	char* text; //not null terminated
	int length; //number of chars
	int position; //current position in buffer

} rstring;

rstring read_file(char* filename);
ated parse(rstring text);
void dealloc_ated(ated object);
char* stringify(ated entityDef);
ated createEntityDef(char* name, int rows, int cols, int states);
void test();

#ifdef TEST
int main(int argc, char *argv[]) {
	test();
}
#endif

void test(){
	printf("running parser test\n");

	rstring buf = read_file("tower.ated");

	ated myentitydef = parse(buf);
	//printfn("Name: %s",myentitydef.name);

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

	printf(stringify(myentitydef));

	///*
	ated newEntidtyDef = createEntityDef("demoland", 75,250,1);
	printf(newEntidtyDef.name);
	char* data = stringify(newEntidtyDef);
	FILE *fp = fopen("demolandnew.ated", "ab");
    if (fp != NULL){
        fputs(data, fp);
        fclose(fp);
    }
	//*/
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
		//length = max index +1
		text.length = ftell (f) ;
		fseek (f, 0, SEEK_SET);
		//note: must free text.text later
		//note: text.text is not null terminated
		text.text = malloc (text.length);
		if (text.text){
			fread (text.text, 1, text.length, f);
		}
		fclose (f);
	}
	//printf(text.text);
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

	//NOTE: temp removed support for backslash escapes
	if(c==92 && false){
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

vec2 find(ated entity, char target){
	for(int r = 0 ; r < entity.rows ; r++){
		for(int c = 0 ; c < entity.columns ; c++){
			if(entity.states[0].hitmap[r * entity.columns + c] == target){
				vec2 found;
				found.x = c;
				found.y = r;
				return found;
			}
		}
	}
	vec2 nope;
	nope.x = -1;
 	nope.y = -1;	
	return nope;
}

ated parse(rstring text){
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
			len+=1;//leaving room for null terminator, man is it a pain to have to remember that
			//TODO look into strncpy and make sure we are really mallocing enough bytes

			//printf("Header: ");
			//fwrite(buffer, 1, len, stdout);
			//printf("\n\n");

			switch(segment) {
				case 0 :
					//everything prior to first delimiter is a comment
					//TODO save this too for stringify
					break;				
				case 1:
					//name
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
					//TODO test this
					{
					//colormapping
					int index = 0;
					int num = 1;//color mapping zero is reserved
					//TODO this could handle errors better
					while(index<len){
						//ex "c|255|000|020|"
						char id = buffer[index];
						color c;
						c.r=strtol(buffer+index+2, NULL, 10);
						c.g=strtol(buffer+index+6, NULL, 10);
						c.b=strtol(buffer+index+10, NULL, 10);
						index+=14;
						entityDef.colormap[id]=num;
						entityDef.colors[num]=c;
						num++;
					}
					break;
					}
				case 9:
					//end of header data, break out of for loop
					header_flg = 0;
					break;

			}
			segment++;
			len=0;
		}else{
			buffer[len]=character;
			len++;
		}
	}

	//text.position = i; DO NOT DO THIS, char gotten from text does not correspond to position due to escapes and newlines
	//Begin Scanning for symbol, color, and hitbox data
	//printfn("%d, %d, %d",entityDef.rows, entityDef.columns, entityDef.block);
	//printfn("Data:%s", data); doesn't work with rstring

	for(int n=0;n<entityDef.num_states;n++){
		//fwrite(data + n*entityDef.block,1,entityDef.block,stdout); putchar('\n');
		//entityDef.states[n].symbols = substring(data + n*entityDef.block, entityDef.block);
		
		//symbols
		entityDef.states[n].symbols = malloc(entityDef.block*sizeof(char));
		for(int i=0; i<entityDef.block;i++){
			entityDef.states[n].symbols[i]=get_byte(&text);
		}
		
		//colormap
		entityDef.states[n].colormap = malloc(entityDef.block*sizeof(char));
		for(int i=0; i<entityDef.block;i++){
			entityDef.states[n].colormap[i]=get_byte(&text);
		}

		//hitmap
		entityDef.states[n].hitmap = malloc(entityDef.block*sizeof(char));
		for(int i=0; i<entityDef.block;i++){
			entityDef.states[n].hitmap[i]=get_byte(&text);
		}
	}

	return entityDef;
}

ated createEntityDef(char* name, int rows, int cols, int states){
	ated newEntidtyDef;
	newEntidtyDef.name = name;
	newEntidtyDef.rows = rows;
	newEntidtyDef.columns = cols;
	newEntidtyDef.num_states = states;
	return newEntidtyDef;
}

char* stringify(ated entityDef){
	char* string = "=\n";
	char buffer[100];
	
	string = concat(string, entityDef.name);
	string = concat(string, "\n=\n");
	string = concat(string, entityDef.display);
	string = concat(string, "\n=\n");
	string = concat(string, entityDef.description);
	string = concat(string, "\n=\n");
	snprintf(buffer,10,"%d",entityDef.id);
	string = concat(string, buffer);
	string = concat(string, "\n=\n");
	snprintf(buffer,10,"%d",entityDef.columns);
	string = concat(string, buffer);
	string = concat(string, "\n=\n");
	snprintf(buffer,10,"%d",entityDef.rows);
	string = concat(string, buffer);
	string = concat(string, "\n=\n");
	snprintf(buffer,10,"%d",entityDef.num_states);
	string = concat(string, buffer);
	string = concat(string, "\n=\n==\n");//TODO ignore color for now

	printfn(string);

	//TODO actually add text
	char* charfill = malloc(sizeof(char) * (entityDef.columns+2));
	char* hitfill = malloc(sizeof(char) * (entityDef.columns+2));
	char* colorfill = malloc(sizeof(char) * (entityDef.columns+2));

	for(int c = 0 ; c<entityDef.columns; c++){
		charfill[c] = 'X';
		hitfill[c] = 'N';
		colorfill[c]='1';
	}

	charfill[entityDef.columns]='\n';
	hitfill[entityDef.columns]='\n';
	colorfill[entityDef.columns]='\n';
	charfill[entityDef.columns+1]='\0';
	hitfill[entityDef.columns+1]='\0';
	colorfill[entityDef.columns+1]='\0';

	for(int s = 0 ; s<entityDef.num_states; s++){
		for(int r = 0 ; r<entityDef.rows; r++){
			string = concat(string, charfill);
		}
		for(int r = 0 ; r<entityDef.rows; r++){
			string = concat(string, colorfill);
		}
		for(int r = 0 ; r<entityDef.rows; r++){
			string = concat(string, hitfill);
		}
	}

	return string;
}

char* concat(char *s1, char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}

void move_vec(uintptr_t pos_loc, int dir){
    vec2 *pos = (vec2 *)pos_loc;
    switch(dir){
        case UP:
            pos->y -= 1;
            break;
        case DOWN:
            pos->y += 1;
            break;
        case RIGHT:
            pos->x += 1;
            break;
        case LEFT:
            pos->x -= 1;
    }
}
#endif
