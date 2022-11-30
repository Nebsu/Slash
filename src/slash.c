#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <glob.h>

#define MAX_ARGS_STRLEN 4096
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

void nb_mots(char *str, commande * cmd) {
    int i = 0;
    int nb = 0;
    bool space = true;
    while (str[i] != '\0') {
        if (str[i] == ' ') {
            space = true;
        }
        else {
            if (space) {
                nb++;
            }
            if(str[i] == '-'){
                cmd->option++;
            }
            space = false;
        }
        i++;
    }
    cmd -> argc = nb;
}

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
    // cmd->argc = mot;
    int i = 0;
    int j = 0;
    int k = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == ' ') {
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
        }
        else {
            j++;
        }
        i++;
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
    cmd->cmd = cmd->args[0];
    return cmd;
}

char * promptFormat() {
    buffer = malloc(MAX_ARGS_STRLEN);
    if(buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
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
    int taille_restant = 30 - 2 - taille + 10;
    int taille_position = strlen(position);

    //Ajoute les ... si la taille de la position est trop grande
    if(taille_position > 27) {
        taille_restant -= 3;
        taille += sprintf(buffer + taille, "...");
    }

    if(taille_position < taille_restant) {
        taille_restant = taille_position;
        taille += sprintf(buffer + taille, "%s", position);
    }
    else {
        taille += sprintf(buffer + taille, "%s", position + taille_position - taille_restant);
    }

    //Ajoute le $ en blanc
    sprintf(buffer + taille, BLANC "$ ");
    return buffer;
}

int main(int argc, char ** argv) {
    if(argc > 1) {
        printf("%sTrop d'arguments%s\n",ROUGE,BLANC);
        return 1;
    }
    rl_outstream = stderr;
    while(1) {
        line = readline (promptFormat());
        if(line == NULL) {
            free(buffer);
            return 0;
        }
        if(strcmp(line, "") == 0) {
            free(buffer);
            free(line);
            continue;
        }
        add_history (line);
        free(buffer);
        commande * cmd = getCommand(line);
        glob_t globbuf;
        globbuf.gl_offs = cmd -> option + 1;
        if(cmd -> argc == cmd -> option + 1) {
            glob("", GLOB_DOOFFS, NULL, &globbuf);
        }
        else {
            glob(cmd->args[cmd -> option + 1], GLOB_DOOFFS, NULL, &globbuf);
        }
        if (strcmp(cmd->cmd, "exit") == 0) {
            if(cmd->argc > 2) {
                printf("%sTrop d'arguments%s\n",ROUGE,BLANC);
                val_retour = 1;
                free_commande(cmd);
                free(line);
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
        else if (strcmp(cmd->cmd, "cd") == 0) {
            val_retour = cd(cmd->argc, cmd->args);
            free_commande(cmd);
        }
        else {
            int n;
            switch (n = fork()) {
                case -1:
                    perror("fork");
                    exit(EXIT_FAILURE);
                case 0:
                    for (int i = 0; i < cmd -> option + 1; i++){
                        globbuf.gl_pathv[i] = cmd -> args[i];
                    }
                    execvp(cmd->cmd, &globbuf.gl_pathv[0]);
                    printf("%sCommande inexistante%s\n",ROUGE,BLANC);
                    val_retour = 127;
                    break;
                default:
                    wait(&n);
                    val_retour = WEXITSTATUS(n);
                    globfree(&globbuf);
                    free_commande(cmd);
                    break;
            }
        
        }
        free(line);
    }
    return val_retour;
}