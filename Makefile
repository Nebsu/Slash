CC ?= gcc
CCFLAGS ?= -g -Wall -std=c99
LINK ?= -lreadline

all: slash pwd cd2

slash: 
	$(CC) src/slash.c -o slash $(LINK) $(CCFLAGS)

# pwd: 
# 	$(CC) $(CCFLAGS) -o bin/pwd  src/pwd.c

pwd:
	$(CC) $(CCFLAGS) -o bin/pwd	src/pwd.c

# exit: 
# 	$(CC) $(CCFLAGS) -o bin/exit  src/exit.c

cd2: 
	$(CC) $(CCFLAGS) -o bin/cd  src/cd.c

clean:
	rm -f bin/* slash