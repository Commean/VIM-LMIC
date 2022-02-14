CFLAGS=-I../../src -lwiringPi -lpthread -lrt -lm -lcrypt
LDFLAGS=-lwiringPi -lpthread -lrt -lm -lcrypt

thethingsnetwork-send-v1: thethingsnetwork-send-v1.cpp
    cd ../../src && $(MAKE)
    $(CC) $(CFLAGS) -o thethingsnetwork-send-v1 thethingsnetwork-send-v1.cpp ../../src/*.o $(LDFLAGS)

all: thethingsnetwork-send-v1

.PHONY: clean

clean:
    rm -f *.o thethingsnetwork-send-v1
