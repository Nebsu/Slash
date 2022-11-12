typedef struct commande {
    char *cmd;
    char **args;
    int argc;
} commande;

void free_commande(commande *cmd) {
    free(cmd->cmd);
    for(int i = 0; i < cmd->argc; i++) {
        free(cmd->args[i]);
    }
    // free(cmd->args);
    free(cmd);
}

commande * init_commande(int argc) {
    commande *cmd = malloc(sizeof(commande));
    if(!cmd) {
        return NULL;
    }
    cmd->cmd = malloc(MAX_ARGS_NUMBER);
    if(!cmd->cmd) {
        free(cmd);
        return NULL;
    }
    cmd->args = malloc(sizeof(char *) * argc);
    for (size_t i = 0; i < argc; i++)
    {
        cmd->args[i] = malloc(MAX_ARGS_STRLEN);
        if(!cmd->args[i]) {
            free_commande(cmd);
            return NULL;
        }
    }
    cmd->argc = argc;
    return cmd;
}

