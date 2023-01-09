#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commande.h"
#include "pipe.h"
#include "util.h"

char deuxCharCmd[2];

/*
    Cette fonction libérer la mémoire allouée à la commande en argument
*/
void free_commande(commande *cmd) {
    free(cmd->cmd);
    for(int i = 1; i < cmd->argc; i++) {
        free(cmd->args[i]);
    }
    free(cmd->args);
    free(cmd);
}

/*
    Cette fonction affiche la commande en argument
*/
void printCom(commande *cmd) {
    printf("cmd =  %s \n",cmd -> cmd);
    printf("args : ");
    for (int i = 0; i < cmd -> argc; i++) {
        printf(" %s ",cmd -> args[i]);
    }
    printf("\n");
}

/*
    Cette fonction permet de libérer la mémoire allouée pour une liste de commande
*/
void free_commande_list(commandeListe * cmdList) {
    for (int i = 0;i < cmdList -> nbCmd; i++) {
        free_commande(cmdList -> cList[i]);
    }
    free(cmdList -> cList);
    free(cmdList);
}

/* 
    Cette fonction permet de transformer une chaine de caractère en une commande
    Elle renvoie la commande si il n'y a pas d'erreur sinon NULL
*/
commande * getCommand(char * buffer) {
    commande * cmd;
    if((cmd = malloc(sizeof(commande))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cmd -> option = 0;
    nb_mots(buffer, cmd);
    if((cmd->args = malloc(sizeof(char *) * (cmd -> argc + 1))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cmd->args[cmd -> argc] = NULL;
    int i = 0;
    int j = 0;
    int k = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == ' ' || buffer[i] == '|') {
            if (buffer[i] == '|' && buffer[i + 2] == '|') {
                return NULL;
            }
            if (j != 0) {
                cmd->args[k] = malloc(sizeof(char) * (j + 1));
                if(cmd->args[k] == NULL) {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }
                strncpy(cmd->args[k], &buffer[i - j], j);
                cmd->args[k][j] = '\0';
                k++;
                j = 0;
            }
            if (buffer[i] == '|') break;
        }
        else {
            strncpy(deuxCharCmd,buffer + i,2);
            if (strcmp(deuxCharCmd,">|") == 0) {
                i++;
                j++;
            }
            j++;
        }
        i++;
    }

    if (j != 0 && buffer[i] != '|') {
        cmd->args[k] = malloc(sizeof(char) * (j + 1));
        if(cmd->args[k] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(cmd->args[k], &buffer[i - j], j);
        cmd->args[k][j] = '\0';
        k++;
        j = 0;
    }
    cmd->cmd = cmd->args[0];
    return cmd;
}

/*
    Cette fonction permet de transformer une chaine de caractère en une liste de commande
    Elle renvoie la liste de commande si il n'y a pas d'erreur sinon NULL
*/
commandeListe * getCommandList(char * buffer) {
    int nbPipe = nbPipes(buffer);
    commandeListe *cmdList;
    cmdList = malloc (sizeof(commandeListe));
    if (!cmdList) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cmdList -> nbCmd = nbPipe + 1;
    cmdList -> cList = malloc(sizeof(commande *) * cmdList -> nbCmd);
    if (!cmdList -> cList) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    int len = strlen(buffer);
    for (int i = 0;i < len;i++) {
        cmdList -> cList[index++] = getCommand(buffer + i);
        if(cmdList -> cList[index-1] == NULL) {
            return NULL;
        }
        while (i < len && buffer[i] != '|') {
            if (i < len - 2) {
                strncpy(deuxCharCmd,buffer + i,2);
                if (strcmp(deuxCharCmd,">|") == 0) {
                    i+=2; 
                    continue;
                }
            }
            i++;
        }
    }
    return cmdList;
}