# Makefile
CC=gcc
LDFLAGS=-Wall -Wextra -Wpedantic -g 
LDLIBS=-lncursesw -lsndfile -lportaudio -lpthread

all: demo parser aquarius
