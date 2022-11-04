CC ?= gcc
CCFLAGS ?= -g -Wall -std=c99

all: slash

slash: 
	$(CC) $(CCFLAGS) -o bin/slash  src/slash.c

# pwd: 
# 	$(CC) $(CCFLAGS) -o bin/pwd  src/pwd.c

# exit: 
# 	$(CC) $(CCFLAGS) -o bin/exit  src/exit.c

# cd: 
# 	$(CC) $(CCFLAGS) -o bin/cd  src/cd.c

clean:
	rm -f bin/*