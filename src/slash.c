#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096
#define MAX_PATH 4096
#define MAX_PRINTED_PATH 24

char buffer[MAX_ARGS_STRLEN];
int val_retour = 0;
// char position[MAX_PATH];


void promptFormat() {
    char * position = "zzzzzzzzzzzzzzzzzzzzzzzzz";
    int taille = 0;
    if (val_retour == 0) {
        printf("\033[32m[0]");
        taille += 3;
    } else {
        printf("\033[91m[1]");
        taille+= 3;
    }
    printf("\033[36m");
    int taille_restant = 30 - taille - 2;
    int taille_position = strlen(position);
    if(taille_position > 25) {
        taille_restant -= 3;
        printf("...");
    }
    if(taille_position < taille_restant) {
        taille_restant = taille_position;
    }
    printf("%.*s", taille_restant, position + taille_position - taille_restant);
    printf("\033[00m$ ");
}

int main(int argc, char ** argv) {
    if(argc > 1) {
        printf("\033[91mTrop d'arguments \033[00m\n");
        return 1;
    }
    promptFormat();
    while(1) {
        scanf("%s", buffer);
        if(strcmp(buffer, "exit") == 0) {
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