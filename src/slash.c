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
#include "util.h"
#include "pipe.h"

static char *line = (char *)NULL;
int val_retour = 0;
char * buffer = NULL;
int sigIntercept = 0;
char buffer_variables[MAX_ARGS_STRLEN];
int changeVal = 0;


/*
    Cette fonction permet d'avoir le chemin actuel ainsi que la valeur de retour
    dans le prompt
*/
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

    //Ajout la fin du prompt
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

/*
    Réponse aux signaux
*/
void handle() {
    sigIntercept = 1;
    val_retour = 255;
    dprintf(2,"Complété");
}

/*
    Ignore les signaux SIGINT et SIGQUIT
*/
void sig_ign() {
     struct sigaction s = {
        .sa_handler = SIG_IGN
    };
    sigaction(SIGINT, &s, NULL);
    sigaction(SIGTERM, &s, NULL);
}

/*
    Gestion de signaux SIGINT et SIGTERM
*/
void sig_hand() {
    struct sigaction s = {
        .sa_handler = handle
    };
    sigaction(SIGINT, &s, NULL);
    sigaction(SIGTERM, &s, NULL);
}


int var_check(char * var) {
    char * egal;
    if ((egal = strchr(var, '=') ) != NULL) {
        if (strlen(egal) > 1) {
            char * val = egal + 1;
            int lenVar = strlen(var) - strlen(egal);
            strncpy(buffer_variables,var,lenVar);
            buffer_variables[lenVar] = '\0';
            setenv(buffer_variables,val,1);
            return 1;
        }
    }
    return 0;
}

int isVar(char * var) {
    return (var[0] == '$');
}

char * var_change(char * var) {
    int i = 1;
    while(var[i] != '\0' && isalnum(var[i])) {
        i++;
    }
    strncpy(buffer_variables,var+1,i-1);
    buffer_variables[i-1] = '\0';
    if (getenv(buffer_variables) != NULL) {
        char * val = getenv(buffer_variables);
        int len = strlen(val) + strlen(var + i) +1;
        char * tmpvar = malloc(len);
        strcpy(tmpvar,val);
        if(var[i] != '\0') {
            strcpy(tmpvar,var + i);
        }
        strncpy(tmpvar,val,strlen(val));
        free(var);
        printf("var = %s \n",tmpvar);
        return tmpvar;
    }
    else {
        return var;
    }
}

int main(int argc, char ** argv) {
    sig_ign();
    if(argc > 1) {
        printf("%sTrop d'arguments%s\n",ROUGE,BLANC);
        return 1;
    }
    rl_outstream = stderr;
    // loadEnv();
    while(1) {
        changeVal = 0;
        int tmpStdin = dup(STDIN_FILENO);
        int tmpStdout = dup(STDOUT_FILENO);
        /*
            Affiche le prompt et récupère la ligne de l'entrée standard
        */
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
        // Traitement de la ligne de commande
        commandeListe * cmdList = getCommandList(line);
        if(cmdList == NULL) {
            free(line);
            val_retour = 2;
            dprintf(2,"Erreur de syntaxe\n");
            continue;
        }
        int ** pipeTab = createPipes(cmdList -> nbCmd);
        /*
            Creation des descripteurs de fichiers modifiables
        */
        int * input_fd = malloc(sizeof(int));
        *input_fd = STDIN_FILENO;
        int * output_fd = malloc(sizeof(int));
        *output_fd = STDOUT_FILENO;
        int * err_fd = malloc(sizeof(int));
        *err_fd = STDERR_FILENO;
        /*
            Gestion des signaux
        */
        sig_hand();
        /*
            Execution des commandes
        */
        for (int i = 0; i < cmdList -> nbCmd; i++) {
             for (int j = 0; j < cmdList -> cList[i] -> argc; j++){
                if (isVar(cmdList -> cList[i] -> args[j])) {
                    cmdList -> cList[i] -> args[j] = var_change(cmdList -> cList[i] -> args[j]);
                }
                else {
                    if(var_check(cmdList -> cList[i] -> args[j])) {
                        if (i == 0 && j == 0 && cmdList -> cList[i] -> argc == 1 && cmdList -> nbCmd == 1) changeVal = 1;
                    }
                    
                    // if(i == 0 && j == 0 && cmdList -> cList[i] -> argc == 1 && cmdList -> nbCmd == 1 ) changeVal = 1;
                }
            }
            char ** buff = star(cmdList -> cList[i] -> argc,cmdList -> cList[i] -> args);
            free(cmdList -> cList[i] -> args);
            /*
                Recréation de la commande après gestion de l'étoile
            */
            cmdList -> cList[i] -> argc = nbc;
            cmdList -> cList[i] -> args = buff;
            cmdList -> cList[i] -> cmd = buff[0];
            /*
                Gestion des redirections
            */
            if (cmdList -> cList[i] -> argc > 2){
                val_retour = redirect(input_fd, output_fd, err_fd, cmdList -> cList[i]);
                if (val_retour == 1){
                    goto end;
                }
            }
            /*
                Gestion des commandes internes
            */
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
                free(input_fd);
                free(output_fd);
                free(err_fd);
                exit(val_retour);
            }
             else if(strcmp(cmdList->cList[i]->cmd, "cd") == 0) {
                val_retour = cd(cmdList->cList[i]->argc, cmdList->cList[i]->args);
            }
            else if(strcmp(cmdList->cList[i]->cmd, "false") == 0) {
                val_retour = 1;
            }
            /*
                Gestion des commande externe (et pwd)
            */
            else {
                int n;
                switch (n = fork()) {
                    case -1:
                    perror("fork");
                    exit(EXIT_FAILURE);
                    case 0 :
                    if(i < cmdList -> nbCmd - 1 ){
                        close(pipeTab[i][0]);
                        dup2(pipeTab[i][1],*output_fd);
                        close(pipeTab[i][1]);
                    }
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
                        free(input_fd);
                        free(output_fd);
                        free(err_fd);
                        if(!changeVal) {
                            execvp(cmdList -> cList[i] -> cmd,cmdList -> cList[i] -> args);
                            dprintf(2,"%sCommande inexistante%s\n",ROUGE, BLANC);
                            freePipes(pipeTab,cmdList -> nbCmd);
                            free_commande_list(cmdList);
                            return 127;
                        }
                        freePipes(pipeTab,cmdList -> nbCmd);
                        free_commande_list(cmdList);
                        return 0;
                    }
                    return val_retour;
                    default :
                    wait(&n);
                    if(sigIntercept ==0 ) val_retour = WEXITSTATUS(n);
                    end:
                    close(pipeTab[i][1]);
                    dup2(pipeTab[i][0],*input_fd);
                    close(pipeTab[i][0]);
                }
            }
        }
        /*
            Libération de la mémoire
        */
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