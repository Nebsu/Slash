typedef struct commande {
    char *cmd;
    char **args;
    int argc;
} commande;

void free_commande(commande *cmd) {
    free(cmd->cmd);
    for(int i = 1; i < cmd->argc; i++) {
        free(cmd->args[i]);
    }
    free(cmd->args);
    free(cmd);
}

