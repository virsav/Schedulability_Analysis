CC = g++
LIBS = -g
CFLAGS = -Wall

all: viraj.cpp
	$(CC) -o assignment4 viraj.cpp -lm -Wall

clean:
	rm -f assignment4