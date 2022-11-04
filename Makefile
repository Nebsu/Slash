CC ?= gcc
CCFLAGS ?= -g -Wall -std=c99

all: slash pwd

slash: 
	$(CC) $(CCFLAGS) -o bin/slash  -c src/slash.c

# pwd: 
# 	$(CC) $(CCFLAGS) -o bin/pwd  -c src/pwd.c

# exit: 
# 	$(CC) $(CCFLAGS) -o bin/exit  -c src/exit.c

# cd: 
# 	$(CC) $(CCFLAGS) -o bin/cd  -c src/cd.c

clean:
	rm -f bin/*