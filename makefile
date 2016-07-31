# Makefile
CC=gcc
LDFLAGS=-Wall -Wextra -Wpedantic -g 
LDLIBS=-lncursesw -lsndfile -lportaudio -lpthread -lpython2.7

all: demo parser aquarius
