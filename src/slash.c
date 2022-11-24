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
    commande * cmd;
    if((cmd = malloc(sizeof(commande))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if((cmd->args = malloc(sizeof(char *) * (mot + 1))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cmd->args[mot] = NULL;
    cmd->argc = mot;
    int i = 0;
    int j = 0;
    int k = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == ' ') {
            if (j != 0) {
                cmd->args[k] = malloc(sizeof(char) * (j + 1));
                strncpy(cmd->args[k], &buffer[i - j], j);
                cmd->args[k][j] = '\0';
                k++;
                j = 0;
            }
        } else {
            j++;
        }
        i++;
    }
    if (j != 0) {
        cmd->args[k] = malloc(sizeof(char) * (j + 1));
        strncpy(cmd->args[k], &buffer[i - j], j);
        cmd->args[k][j] = '\0';
        k++;
        j = 0;
    }
    cmd->cmd = cmd->args[0];
    return cmd;
}

char * promptFormat() {
    buffer = malloc(MAX_ARGS_STRLEN);
    char * position = getenv("PWD");
    int taille = 0;

    //Ajoute la valeur de retour 
    if (val_retour == 0) {
        taille += sprintf(buffer, VERT);
    }
    else {
        taille += sprintf(buffer, ROUGE);
    }
    taille += sprintf(buffer + taille, "[%d]%s", val_retour, BLEU);
    int taille_restant = 30 - 3 - taille + 10;
    int taille_position = strlen(position);

    //Ajoute les ... si la taille de la position est trop grande
    if(taille_position > 27) {
        taille_restant -= 3;
        taille += sprintf(buffer + taille, "...");
    }

    if(taille_position < taille_restant) {
        taille_restant = taille_position;
        taille += sprintf(buffer + taille, "%s", position);
    }else{
        taille += sprintf(buffer + taille, "%s", position + taille_position - taille_restant);
    }

    //Ajoute le $ en blanc
    strcat(buffer, "\033[00m$ ");
    return buffer;
}

int main(int argc, char ** argv) {
    if(argc > 1) {
        printf("\033[91mTrop d'arguments \033[00m\n");
        return 1;
    }
    rl_outstream = stderr;
    while(1) {
        line = readline (promptFormat());
        if(strcmp(line, "") == 0) {
            free(buffer);
            continue;
        }
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
                free_commande(cmd);
        }
        else{
            printf("Commande inexistante\n");
            val_retour = 1;
        }
        free(line);
    }
    return val_retour;
}