CC ?= gcc
CCFLAGS ?= -g -Wall -std=c99
LINK ?= -lreadline

all: slash

slash: 
	$(CC) src/slash.c -o bin/slash $(LINK) $(CCFLAGS)

# pwd: 
# 	$(CC) $(CCFLAGS) -o bin/pwd  src/pwd.c

# exit: 
# 	$(CC) $(CCFLAGS) -o bin/exit  src/exit.c

# cd: 
# 	$(CC) $(CCFLAGS) -o bin/cd  src/cd.c

clean:
	rm -f bin/*