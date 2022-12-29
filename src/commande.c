#include <stdio.h>
#include <stdlib.h>
#include "commande.h"

void free_commande(commande *cmd) {
    free(cmd->cmd);
    for(int i = 1; i < cmd->argc; i++) {
        free(cmd->args[i]);
    }
    free(cmd->args);
    free(cmd);
}

void printCom(commande *cmd) {
    printf("cmd =  %s \n",cmd -> cmd);
    printf("args : ");
    for (int i = 0; i < cmd -> argc; i++) {
        printf(" %s ",cmd -> args[i]);
    }
    printf("\n");
}

void free_commande_list(commandeListe * cmdList) {
    for (int i = 0;i < cmdList -> nbCmd; i++) {
        free_commande(cmdList -> cList[i]);
    }
    free(cmdList -> cList);
    free(cmdList);
}