typedef struct commande {
    char *cmd;
    char **args;
    int argc;
    int option;
} commande;

void free_commande(commande *cmd) {
    free(cmd->cmd);
    for(int i = 1; i < cmd->argc; i++) {
        free(cmd->args[i]);
    }
    free(cmd->args);
    free(cmd);
}

typedef struct commandeListe {
    commande **cList;
    int nbCmd;
} commandeListe;

void printCom(commande *cmd) {
    printf("cmd =  %s \n",cmd -> cmd);
    printf("args : ");
    for (int i = 0; i < cmd -> argc; i++) {
        printf(" %s ",cmd -> args[i]);
    }
    printf("\n");
}