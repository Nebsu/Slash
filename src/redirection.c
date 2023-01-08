#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "redirection.h"


/* 
   Cette fonction permet de rediriger les descripteurs selon la redirection 
   et modifie la commande sans les éléments de redirection
*/
int redirect(int * input_fd, int * output_fd, int * err_fd, commande * cmd){
    for (int i = 0; i < cmd -> argc; i++) {
        if (strcmp(cmd -> args[i], "<") == 0 && *input_fd == 0) {
            *input_fd = open(cmd -> args[i + 1], O_RDONLY);
            if (*input_fd == -1) {
                perror("open");
                return 1;
            }
        }else if (strcmp(cmd -> args[i], ">") == 0) {
            if (cmd -> args[i + 1] == NULL || cmd -> args[i + 1][0] == '|') {
                dprintf(2,"Erreur de syntaxe : '>' doit être suivi d'un nom de fichier\n");
                return 1;
            };
            *output_fd = open(cmd -> args[i + 1], O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, 0664);
            if (*output_fd == -1) {
                perror("open");
                return 1;
            }
        }else if (strcmp(cmd -> args[i], ">>") == 0) {
            *output_fd = open(cmd -> args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0664);
            if (*output_fd == -1) {
                perror("open");
                return 1;
            }
        }else if (strcmp(cmd -> args[i], ">|") == 0) {
            *output_fd = open(cmd -> args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (*output_fd == -1) {
                perror("open");
                return 1;
            }
        }else if (strcmp(cmd -> args[i], "2>") == 0) {
            *err_fd = open(cmd -> args[i + 1], O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, 0664);
            if (*err_fd == -1) {
                perror("open");
                return 1;
            }  
        }else if (strcmp(cmd -> args[i], "2>>") == 0) {
            *err_fd = open(cmd -> args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0664);
            if (*err_fd == -1) {
                perror("open");
                return 1;
            }

        }else if (strcmp(cmd -> args[i], "2>|") == 0) {
            *err_fd = open(cmd -> args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (*err_fd == -1) {
                perror("open");
                return 1;
            }
        } else {
            continue;
        }
        for (int j = i; j < cmd -> argc - 2; j++) {
            cmd -> args[j] = cmd -> args[j + 2];
        }
        cmd -> argc -= 2;
        cmd -> args[cmd -> argc] = NULL;
        i--;
    }
    return 0;
}