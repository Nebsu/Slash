
# Slash

Notre programme "Slash" permet d'interpréter les commandes comme dans un shell.

## Structure

Ce projet est composé de 8 fichiers sources :

### slash
slash est le coeur du programme, il permet d'interpréter les entrées de l'utilisateur, traiter
ces informations puis renvoie dans le terminal le résultat.
slash inclut tous les fichiers sources du projet afin d'effectuer le traitement des informations
pour appeler les commandes réimplémentés "pwd", "cd" ainsi que les commandes externes.

#### Couleurs

| Utilisation             | Couleur                                                                |
| ----------------- | ------------------------------------------------------------------ |
| Texte | ![#FFFFFF](https://via.placeholder.com/10/FFFFFF?text=+)|
| Chemin | ![#2a9cb5](https://via.placeholder.com/10/2a9cb5?text=+)|
| Succès | ![#208434](https://via.placeholder.com/10/208434?text=+)|
| Erreur | ![#d23c3c](https://via.placeholder.com/10/d23c3c?text=+)|

### commande
commande regroupe les fonctions qui transforment la ligne de commande entrée par l'utilisateur et la transforme en une 
structure "commande" qui permet de traiter la ligne mot par mot et permet d'avoir la liste des arguments pour exécuter les commandes.
Il permet également de séparer les commandes afin d'avoir une liste de commande. Ce qui est particulièrement utile
pour le traitement des "pipelines".
```c
typedef struct commande {
    char *cmd;
    char **args;
    int argc;
    int option;
} commande;


typedef struct commandeListe {
    commande **cList;
    int nbCmd;
} commandeListe;
```

### util
util regroupe des fonctions usuelles comme compter le nombre de mots...

### cd
cd rassemble les fonctions permettant de reproduire la commande "cd" de bash.

### pwd
pwd rassemble les fonctions permettant de reproduire la commande "pwd" de bash.

### star
star transforme les "*" comme dans bash dans les cas suivants : 
- "\*" en prefixe permet trouver les fichiers terminant par ce qui ce trouver après "\*"
- "*" tout seul liste tous les fichiers se trouvant dans le répertoire courant
- "**" permet d'aller dans tous les répertoires se trouvant à la fin de l'arborescence

### redirection
redirection permet de détecter la présence d'une redirection et change la sortie en conséquence

### pipe
pipe sert à simuler "|" de bash, qui permet de lancer plusieurs commandes   à la suite sur une même sortie

## Architecture

![Diagramme](/diagramme.png)