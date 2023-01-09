#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "pipe.h"

char deuxCharPipe[2];

int ** createPipes(int n) {
    int **pipeTab = malloc(n*sizeof(int *));
    for (int i = 0;i < n ; i++) {
        pipeTab[i] = malloc (2 * sizeof(int));
        if (pipe(pipeTab[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
    return pipeTab;
}

void freePipes(int ** pipeTab,int n) {
    for (int i = 0;i < n; i++) {
        free(pipeTab[i]);
    }
    free(pipeTab);
}

int nbPipes(char * buffer) {
    int i = 0;
    int acc = 0;
    int len = strlen(buffer);
    while(buffer[i] != '\0') {
        if (i < len -1) {
            strncpy(deuxCharPipe,buffer + i,2);
            if (strcmp(deuxCharPipe,">|") == 0) {
                i+=2; continue;
            }
        }
        if (buffer[i] == '|') acc++;
        i++;
    }
    return acc; 
}