CC ?= gcc
CCFLAGS ?= -g -Wall -std=gnu17
LINK ?= -lreadline

all: slash

slash: slash.o commande.o cd.o pwd.o star.o redirection.o util.o pipe.o
	$(CC) $(CCFLAGS) src/slash.c bin/commande.o bin/pwd.o bin/cd.o bin/star.o bin/redirection.o bin/util.o bin/pipe.o -o slash $(LINK)

slash.o: src/slash.c 
	$(CC) -c -o bin/slash.o src/slash.c  $(LINK) $(CCFLAGS)

pwd.o: src/pwd.h src/pwd.c
	$(CC) $(CCFLAGS) -c -o bin/pwd.o src/pwd.c

cd.o: src/cd.c src/cd.h
	$(CC) $(CCFLAGS) -c -o bin/cd.o src/cd.c

star.o: src/star.c src/star.h
	$(CC) $(CCFLAGS) -c -o bin/star.o src/star.c

commande.o: src/commande.c src/commande.h
	$(CC) $(CCFLAGS) -c -o bin/commande.o src/commande.c

redirection.o: src/redirection.c src/redirection.h
	$(CC) $(CCFLAGS) -c -o bin/redirection.o src/redirection.c

util.o: src/util.c src/util.h
	$(CC) $(CCFLAGS) -c -o bin/util.o src/util.c

pipe.o: src/pipe.c src/pipe.h
	$(CC) $(CCFLAGS) -c -o bin/pipe.o src/pipe.c

clean:
	rm -f bin/* slash