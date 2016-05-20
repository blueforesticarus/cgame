// Common functions used in this project.

#include "datatypes.h"

/* Parser Functions */

void crocodile_test(){
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
			len+=1;//leaving room for null terminator, man is it a pain to have to remember that
			//TODO look into strncpy and make sure we are really mallocing enough bytes

			printf("Header: ");
			fwrite(buffer, 1, len, stdout);
			printf("\n\n");

			switch(segment) {
				case 0 :
					//everything prior to first delimiter is a comment	
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
				// TODO: Spaker, please add a default case.
				// I'm not really sure what it should be.

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
	printfn("%d, %d, %d",entityDef.rows, entityDef.columns, entityDef.block);
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


/* Sound Functions */
int Callback(const void *input,
             void *output,
             unsigned long frameCount,
             const PaStreamCallbackTimeInfo* paTimeInfo,
             PaStreamCallbackFlags statusFlags,
             void *userData)
{
  adata_t *data = (adata_t *)userData; /* we passed a data structure into the callback so we have something to work with */
  int *cursor; /* current pointer into the output  */
  int *inject_buff;
  int *out = (int *)output;
  int thisSize = frameCount;
  int thisRead;

  cursor = out; /* set the output cursor to the beginning */
  while (thisSize > 0)
  {
    /* seek to our current file position */
    sf_seek(data->sndFile, data->position, SEEK_SET);

    /* are we going to read past the end of the file?*/
    if (thisSize > (data->sfInfo.frames - data->position))
    {
      /*if we are, only read to the end of the file*/
      thisRead = data->sfInfo.frames - data->position;
      /* and then loop to the beginning of the file */
      data->position = 0;
    }
    else
    {
      /* otherwise, we'll just fill up the rest of the output buffer */
      thisRead = thisSize;
      /* and increment the file position */
      data->position += thisRead;
    }

    /* since our output format and channel interleaving is the same as sf_readf_int's requirements */
    /* we'll just read straight into the output buffer */
    
	sf_readf_int(data->sndFile, cursor, thisRead);
	int i;
	for (i = 0; i < thisRead; i++)
	{
		if (data->volch)
		{
			if (data->deltavol > 0)
			{
				data->volume += 0.001;
				data->deltavol -= 0.001;
			}

			if (data->deltavol < 0)
			{
				data->volume -= 0.001;
				data->deltavol += 0.001;
			}

			if (data->deltavol == 0)
			{
				data->volch = 0;
			}
		}
		
		cursor[i] *= data->volume;
	}
    /* increment the output cursor*/
    cursor += thisRead;
    /* decrement the number of samples left to process */
    thisSize -= thisRead;
  }

  return paContinue;
}


void handle_sound(adata_t *data)
{
  PaStream *stream;
  PaError error;
  PaStreamParameters outputParameters;

  /* initialize our data structure */
  data->position = 0;
  data->sfInfo.format = SF_FORMAT_OGG;
  /* try to open the file */
  data->sndFile = sf_open(data->filepath, SFM_READ, &data->sfInfo);

  if (!data->sndFile)
  {
    printf("error opening file\n");
    exit(1);
  }

  /* start portaudio */
  Pa_Initialize();

  /* set the output parameters */
  outputParameters.device = Pa_GetDefaultOutputDevice(); /* use the default device */
  outputParameters.channelCount = data->sfInfo.channels; /* use the same number of channels as our sound file */
  outputParameters.sampleFormat = paInt32; /* 32bit int format */
  outputParameters.suggestedLatency = 0.2; /* 200 ms ought to satisfy even the worst sound card */
  outputParameters.hostApiSpecificStreamInfo = 0; /* no api specific data */

  /* try to open the output */
  error = Pa_OpenStream(&stream,  /* stream is a 'token' that we need to save for future portaudio calls */
                        0,  /* no input */
                        &outputParameters,
                        data->sfInfo.samplerate,  /* use the same sample rate as the sound file */
                        paFramesPerBufferUnspecified,  /* let portaudio choose the buffersize */
                        paNoFlag,  /* no special modes (clip off, dither off) */
                        Callback,  /* callback function defined above */
                        data ); /* pass in our data structure so the callback knows what's up */

  /* if we can't open it, then bail out */
  if (error)
  {
    printf("error opening output, error code = %i\n", error);
    Pa_Terminate();
  }

  /* when we start the stream, the callback starts getting called */
  Pa_StartStream(stream);
  long ms_length = ((double)data->sfInfo.frames / (double)data->sfInfo.samplerate) * 1000.;
  Pa_Sleep(ms_length); 
  Pa_CloseStream(stream); // stop the stream
  (*data).thread_complete = 1;
  Pa_Terminate(); // and shut down portaudio
}

void inject_audio(adata_t* adata, char* filepath)
{
	adata->injInfo.format = SF_FORMAT_OGG;
	adata->injFile = sf_open(filepath, SFM_READ, &adata->injInfo);
	adata->inject_audio = 1;
}

/* syscomms functions */

void killer(int dummy){
 run=0;
}

/* ncurses helpers */
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

void genstars(int num){
    int i;
    for(i=0;i<num;i++){
        stars[i].x=random_number(maxx/6 + 2, 5*maxx/6 - 1);
        stars[i].y=random_number(0,maxy);
    }
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

/* Misc. */
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
typedef struct myStruct someStruct;
  strcat(mystring, &pop);
 }

 return mystring;
}


