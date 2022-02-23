CFLAGS=-I../../lmic -lwiringPi -lpthread -lrt -lm -lcrypt
LDFLAGS=-lwiringPi -lpthread -lrt -lm -lcrypt

thethingsnetwork-send-v2: thethingsnetwork-send-v2.cpp
	cd ../../lmic && $(MAKE)
	$(CC) $(CFLAGS) -o thethingsnetwork-send-v2 thethingsnetwork-send-v2.cpp ../../lmic/*.o $(LDFLAGS)

all: thethingsnetwork-send-v2

.PHONY: clean

clean:
	rm -f *.o thethingsnetwork-send-v2
