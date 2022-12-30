#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_ARGS_STRLEN 4096
#define ROUGE "\033[91m"
#define VERT "\033[32m"
#define BLEU "\033[36m"
#define BLANC "\033[00m"

#include "commande.h"
#include "pwd.h"
#include "cd.h"
#include "star.h"
#include "redirection.h"

static char *line = (char *)NULL;
int val_retour = 0;
char * buffer = NULL;
char deuxChar [2];
int sigIntercept = 0;

void nb_mots(char *str, commande * cmd) {
    int i = 0;
    int nb = 0;
    bool space = true;
    while (str[i] != '\0' && str[i] != '|') {
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
        strncpy(deuxChar,str + i,2);
        if (strcmp(deuxChar,">|") == 0) {
            i+=2; continue;
        }
        i++;
    }
    cmd -> argc = nb;
}

int nbChar(char * buffer,char c) {
    int i = 0;
    int acc = 0;
    while(buffer[i] != '\0') {
        if (buffer[i] == c) acc++;
        i++;
    }
    return acc;
}

int nbPipes(char * buffer) {
    int i = 0;
    int acc = 0;
    int len = strlen(buffer);
    while(buffer[i] != '\0') {
        if (i < len -1) {
            strncpy(deuxChar,buffer + i,2);
            if (strcmp(deuxChar,">|") == 0) {
                i+=2; continue;
            }
        }
        if (buffer[i] == '|') acc++;
        i++;
    }
    return acc; 
}

int ** createPipes(int n) {
    int **pipeTab = malloc(n*sizeof(int *));
    for (int i = 0;i < n ; i++) {
        pipeTab[i] = malloc (2 * sizeof(int));
        if (pipe(pipeTab[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
    return pipeTab;
}

void freePipes(int ** pipeTab,int n) {
    for (int i = 0;i < n; i++) {
        free(pipeTab[i]);
    }
    free(pipeTab);
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
        if (buffer[i] == ' ' || buffer[i] == '|') {
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
            strncpy(deuxChar,buffer + i,2);
            if (strcmp(deuxChar,">|") == 0) {
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
        while (i < len && buffer[i] != '|') {
            if (i < len - 2) {
                strncpy(deuxChar,buffer + i,2);
                if (strcmp(deuxChar,">|") == 0) {
                    i+=2; continue;
                }
            }
            i++;
        }
    }
    return cmdList;
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
    if (sigIntercept == 0 )taille += sprintf(buffer + taille, "[%d]%s", val_retour, BLEU);
    else taille += sprintf(buffer + taille, "[SIG]%s", BLEU);
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

void handle() {
    sigIntercept = 1;
    val_retour = 255;
    printf("val ret = %d \n",val_retour);
}

void sig_ign() {
     struct sigaction s = {
        .sa_handler = SIG_IGN
    };
    sigaction(SIGINT, &s, NULL);
    sigaction(SIGTERM, &s, NULL);
}

void sig_hand() {
    struct sigaction s = {
        .sa_handler = handle
    };
    sigaction(SIGINT, &s, NULL);
    sigaction(SIGTERM, &s, NULL);
}

int main(int argc, char ** argv) {
    sig_ign();
    if(argc > 1) {
        printf("%sTrop d'arguments%s\n",ROUGE,BLANC);
        return 1;
    }
    rl_outstream = stderr;
    while(1) {
        int tmpStdin = dup(STDIN_FILENO);
        int tmpStdout = dup(STDOUT_FILENO);
        line = readline (promptFormat());
        sigIntercept = 0;
        if(line == NULL) {
            free(buffer);
            return val_retour;
        }
        if(strcmp(line, "") == 0) {
            free(buffer);
            free(line);
            continue;
        }
        add_history (line);
        free(buffer);
        commandeListe * cmdList = getCommandList(line);
        int ** pipeTab = createPipes(cmdList -> nbCmd);
        int * input_fd = malloc(sizeof(int));
        *input_fd = STDIN_FILENO;
        int * output_fd = malloc(sizeof(int));
        *output_fd = STDOUT_FILENO;
        int * err_fd = malloc(sizeof(int));
        *err_fd = STDERR_FILENO;
        sig_hand();
        for (int i = 0; i < cmdList -> nbCmd; i++) {
            if (cmdList -> cList[i] -> argc > 2){
                val_retour = redirect(input_fd, output_fd, err_fd, cmdList -> cList[i]);
                if (val_retour == 1){
                    goto end;
                }
            } 
            if (strcmp(cmdList->cList[i]->cmd, "exit") == 0) {
                if(cmdList->cList[i]->argc > 2) {
                    printf("%sTrop d'arguments%s\n",ROUGE,BLANC);
                    val_retour = 1;
                    continue;
                }
                else if(cmdList->cList[i]->argc != 1) {
                    val_retour = atoi(cmdList->cList[i]->args[1]);
                }
                freePipes(pipeTab,cmdList -> nbCmd);
                free_commande_list(cmdList);
                free(line);
                exit(val_retour);
            }
             else if(strcmp(cmdList->cList[i]->cmd, "cd") == 0) {
                val_retour = cd(cmdList->cList[i]->argc, cmdList->cList[i]->args);
            }
            else if(strcmp(cmdList->cList[i]->cmd, "false") == 0) {
                val_retour = 1;
            }
            else {
                int n;
                switch (n = fork()) {
                    case -1:
                    perror("fork");
                    exit(EXIT_FAILURE);
                    case 0 :
                    if(i < cmdList -> nbCmd - 1 ){
                        close(pipeTab[i][0]);
                        dup2(pipeTab[i][1],1);
                        close(pipeTab[i][1]);
                    }
                    // char ** buff = star(cmdList -> cList[i] -> argc,cmdList -> cList[i] -> args);
                    if(strcmp(cmdList->cList[i]->cmd, "pwd") == 0) {
                        val_retour = pwd(cmdList->cList[i]->argc, cmdList->cList[i]->args, output_fd);
                        free(input_fd);
                        free(output_fd);
                        free(err_fd);
                        freePipes(pipeTab,cmdList -> nbCmd);
                        free_commande_list(cmdList);
                    }
                    else {
                        dup2(*input_fd, STDIN_FILENO);
                        dup2(*output_fd, STDOUT_FILENO);
                        dup2(*err_fd, STDERR_FILENO);
                        execvp(cmdList -> cList[i] -> cmd,cmdList -> cList[i] -> args);
                        printf("%sCommande inexistante%s\n",ROUGE,BLANC);
                        freePipes(pipeTab,cmdList -> nbCmd);
                        free_commande_list(cmdList);
                        return 127;
                    }
                    return val_retour;
                    default :
                    wait(&n);
                    if(sigIntercept ==0 ) val_retour = WEXITSTATUS(n);
                    end:
                    close(pipeTab[i][1]);
                    dup2(pipeTab[i][0],0);
                    close(pipeTab[i][0]);
                }
            }
        }
        free(input_fd);
        free(output_fd);
        free(err_fd);
        sig_ign();
        dup2(tmpStdin,STDIN_FILENO);
        dup2(tmpStdout,STDOUT_FILENO);
        freePipes(pipeTab,cmdList -> nbCmd);
        free_commande_list(cmdList);
        free(line);
    }
    return val_retour;
} 