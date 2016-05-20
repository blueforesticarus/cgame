# Makefile
LDFLAGS=-Wall -Wextra -Wpedantic -g -lncursesw -lsndfile -lportaudio -lpthread

all: demo parser aquarius
