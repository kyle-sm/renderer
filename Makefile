CC = tcc
LIBS = -lglfw -lGLEW 
CFLAGS = -O3 -Wall -std=c99
SRC	= render.c

all:
	$(CC) $(LIBS) $(CFLAGS) -o gl $(SRC)