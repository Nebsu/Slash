CC ?= gcc
CCFLAGS ?= -g -Wall -std=c99
LINK ?= -lreadline

all: slash

slash : slash.o cd.o pwd.o
	$(CC) $(CCFLAGS) src/slash.c bin/pwd.o bin/cd.o -o slash $(LINK)

slash.o: src/slash.c 
	$(CC) -c -o bin/slash.o src/slash.c  $(LINK) $(CCFLAGS)

pwd.o: src/pwd.h src/pwd.c
	$(CC) $(CCFLAGS) -c -o bin/pwd.o src/pwd.c

cd.o: src/cd.c src/cd.h
	$(CC) $(CCFLAGS) -c -o bin/cd.o src/cd.c

clean:
	rm -f bin/* slash