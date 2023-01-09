#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "commande.h"
#include "util.h"

/*
    Cette fonction permet de compter le nombre de mots dans une chaine de caractères
    et les ajoute dans argc de cmd
*/
void nb_mots(char *str, commande * cmd) {
    int i = 0;
    int nb = 0;
    char deuxChar[2];
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


/*
    Cette fonction permet de compter le nombre d'occurence de c dans buffer
    Elle renvoie le nombre d'occurence
*/
int nbChar(char * buffer,char c) {
    int i = 0;
    int acc = 0;
    while(buffer[i] != '\0') {
        if (buffer[i] == c) acc++;
        i++;
    }
    return acc;
}