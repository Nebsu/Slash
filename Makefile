CC ?= gcc
CCFLAGS ?= -g -Wall -std=gnu17
LINK ?= -lreadline

all: slash

slash: slash.o cd.o pwd.o star.o mystring.o pile.o
	$(CC) $(CCFLAGS) src/slash.c bin/pwd.o bin/cd.o -o slash $(LINK)

slash.o: src/slash.c 
	$(CC) -c -o bin/slash.o src/slash.c  $(LINK) $(CCFLAGS)

pwd.o: src/pwd.h src/pwd.c
	$(CC) $(CCFLAGS) -c -o bin/pwd.o src/pwd.c

cd.o: src/cd.c src/cd.h
	$(CC) $(CCFLAGS) -c -o bin/cd.o src/cd.c

pile.o: src/pile.c src/pile.h
	$(CC) $(CCFLAGS) -c -o bin/pile.o src/pile.c

mystring.o: src/mystring.c src/mystring.h
	$(CC) $(CCFLAGS) -c -o bin/mystring.o src/mystring.c

star.o: src/star.c src/star.h
	$(CC) $(CCFLAGS) -c -o bin/star.o src/star.c

clean:
	rm -f bin/* slash