// Common prototypes for functions used in this project.

#ifndef COMMON_H
#define COMMON_H

#include "datatypes.h"

/* sys communication functions */
void killer(int dummy);

/* Parser Functions */
rstring read_file(char* filename);
ated parse(rstring text);
void dealloc_ated(ated object);
void test();
char* substring(char* src, int len);
void printfn(const char *fmt, ...);
void crocodile_test();
char get_byte(rstring* buff);

/* Sound Handling Functions */
void handle_sound(adata_t* data);
void inject_audio(adata_t* adata, char* filepath);
int Callback(const void *input,
             void *output,
             unsigned long frameCount,
             const PaStreamCallbackTimeInfo* paTimeInfo,
             PaStreamCallbackFlags statusFlags,
             void *userData);

/* ncurses helpers */
char *randstring(int length);
void genstars(int num);
void handle_winch(int sig);

/* Misc. */
int random_number(int min_num, int max_num);
char *getstring(int length);


#endif
