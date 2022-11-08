#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <readline/readline.h>
#include <readline/history.h>


#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096
#define MAX_PATH 4096
#define MAX_PRINTED_PATH 24

static char *line = (char *)NULL;
int val_retour = 0;

void promptFormat() {
    char * position = "zzzzzzzzzzzzzzzzzzzzzzzzz";
    int taille = 0;
    if (val_retour == 0) {
        fprintf(stderr,"\033[32m[0]");
        taille += 3;
    } else {
        fprintf(stderr,"\033[91m[1]");
        taille+= 3;
    }
    fprintf(stderr,"\033[36m");
    int taille_restant = 30 - taille - 2;
    int taille_position = strlen(position);
    if(taille_position > 25) {
        taille_restant -= 3;
        fprintf(stderr,"...");
    }
    if(taille_position < taille_restant) {
        taille_restant = taille_position;
    }
    fprintf(stderr,"%.*s", taille_restant, position + taille_position - taille_restant);
    fprintf(stderr,"\033[00m$ ");
}

int main(int argc, char ** argv) {
    if(argc > 1) {
        printf("\033[91mTrop d'arguments \033[00m\n");
        return 1;
    }
    promptFormat();
    rl_outstream = stderr;
    while(1) {
        line = readline ("");
        add_history (line);
        if(strcmp(line, "exit") == 0) {
            free(line);
            return 0;
        }
        else {
            val_retour = 1;
            printf("Commande inconnue\n");
            promptFormat();
        }
    }
    return 0;
}