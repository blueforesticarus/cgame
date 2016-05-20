//Common Datatypes used in this project.

#ifndef DATATYPES_H
#define DATATYPES_H
#include <sndfile.h>
#include <portaudio.h>
/* Generally useful structs. */

typedef struct {
	int x;
	int y;
	} vec2;

typedef struct {
	int x;
	int y;
	int z;
	} vec3;

/* Datatypes for ASCII display */

// Struct contains information about an ASCII Text Entity Descriptor
typedef struct {
	//note that none of the following are null terminated
	//as NULL (zero byte) may be a valid character
	char* symbols;	//characters
	char* hitmap;	//hitmap
	char* colormap;//colors

	vec2 center;	//center of block for positioning
	char* name;	//name of state
	} ated_state;

typedef struct {
	short r;
	short g;
	short b;
	} color;

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

/* Datatypes for use with PortAudio and LibSNDfile */

// Struct contians information about some audio data to be played.
typedef struct{
	SNDFILE *sndFile, *injFile; 	// (Experimental) Secondary audio file to be played overtop the primary
	SF_INFO sfInfo, injInfo; 		// Information about the audio file to be played
	volatile char* filepath;		// Filepath to audio to be played.
	volatile int inject_audio;		// (Experimental) Flag whether or not we are injecting audio
	int position;					// Index into audio file
	volatile double volume;
	volatile double deltavol;		// Portaudio docs suggest that gradually changing the volume may fix the pops.
	volatile int volch;				// Flag to see if volume is changing.
	volatile int thread_complete;	// Simple semaphore to show the audio thread is finished.
	} adata_t;

#endif
