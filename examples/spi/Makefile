CPPFLAGS=-lwiringPi
LDFLAGS=-lwiringPi -lpthread -lrt -lm -lcrypt
CC = g++

main: main.cpp
	$(CC) $(CPPFLAGS) -o main main.cpp $(LDFLAGS)

all: main

.PHONY: clean

clean:
	rm -f *.o main
