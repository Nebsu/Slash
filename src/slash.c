#define _DEFAULT_SOURCE
#define  _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <unistd.h>
#include <wait.h>

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096
#define MAX_PATH 4096
#define MAX_PRINTED_PATH 24
#define ROUGE "\033[91m"
#define VERT "\033[32m"
#define BLEU "\033[36m"
#define BLANC "\033[00m"

#include "commande.h"
#include "pwd.h"
#include "cd.h"

static char *line = (char *)NULL;
int val_retour = 0;
char * buffer = NULL;

int nb_mots(char *str) {
    int i = 0;
    int nb = 0;
    bool space = true;
    if(str[0] == ' ') {
        space = false;
    }
    while (str[i] != '\0') {
        if (str[i] == ' ') {
            space = true;
        } else {
            if (space) {
                nb++;
            }
            space = false;
        }
        i++;
    }
    return nb;
}

commande * getCommand(char * buffer) {
    int mot = nb_mots(buffer);
    if(mot == 0) {
        return NULL;
    }
    commande * cmd = malloc(sizeof(commande));
    if(!cmd) {
        return NULL;
    }
    cmd->argc = mot;
    cmd->args = malloc(sizeof(char *) * mot);
    if(!cmd->args) {
        free(cmd);
        return NULL;
    }
    int debut = 0;
    int fin = 0;
    int i = 0;
    while(buffer[fin] != '\0') {
        if(buffer[fin] == ' ') {
            cmd->args[i] = malloc(fin - debut + 1);
            if(!cmd->args[i]) {
                return NULL;
            }
            strncpy(cmd->args[i], buffer + debut, fin - debut);
            cmd->args[i][fin - debut] = '\0';
            debut = fin + 1;
            i++;
        }
        fin++;
    }
    cmd->args[i] = malloc(fin - debut + 1);
    if(!cmd->args[i]) {
        return NULL;
    }
    strncpy(cmd->args[i], buffer + debut, fin - debut);
    cmd->args[i][fin - debut] = '\0';
    cmd->args[i + 1] = NULL;
    cmd->cmd = cmd->args[0];
    return cmd;
}

char * promptFormat() {
    buffer = malloc(MAX_ARGS_STRLEN);
    char * position = getenv("PWD");
    int taille = 0;

    //Ajoute la valeur de retour
    if (val_retour == 0) {
        strcat(buffer, "\033[32m[0]\033[36m");
        taille += 3;
    } else {
        strcat(buffer,"\033[91m[1]\033[36m");
        taille += 3;
    }
    int taille_restant = 30 - taille - 2;
    int taille_position = strlen(position);

    //Ajoute les ... si la taille de la position est trop grande
    if(taille_position > 25) {
        taille_restant -= 3;
        strcat(buffer, "...");
    }
    if(taille_position < taille_restant) {
        taille_restant = taille_position;
    }

    //Ajoute le chemin
    char * tmp = position + taille_position - taille_restant;
    strcat(buffer, tmp);

    //Ajoute le $ en blanc
    strcat(buffer, "\033[00m$ ");
    return buffer;
}

int main(int argc, char ** argv) {
    // printf(getenv("PWD"));
    int n;
    if(argc > 1) {
        printf("\033[91mTrop d'arguments \033[00m\n");
        return 1;
    }
    rl_outstream = stderr;
    while(1) {
        line = readline (promptFormat());
        add_history (line);
        free(buffer);
        commande * cmd = getCommand(line);
        if (strcmp(cmd->cmd, "exit") == 0) {
            if(cmd->argc > 2) {
                printf("\033[91mTrop d'arguments \033[00m\n");
                val_retour = 1;
                free_commande(cmd);
                continue;
            }
            else if(cmd->argc != 1) {
                val_retour = atoi(cmd->args[1]);
            }
            free_commande(cmd);
            break;
        }
        else if(strcmp(cmd->cmd, "pwd") == 0) {
            val_retour = pwd(cmd->argc, cmd->args);
            free_commande(cmd);
        }
        else if (strcmp(cmd->cmd, "cd") == 0){
            val_retour = cd(cmd->argc, cmd->args);
            if(cmd ->argc > 2 && strcmp(cmd->args[1], "-P") == 0  && strcmp(cmd->args[2], "..") == 0) {
            }
            else {
                free_commande(cmd);
            }
        }
        else{
            printf("Commande inexistante\n");
            val_retour = 1;
        }

    }
    return val_retour;
}