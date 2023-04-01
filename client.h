#ifndef __ECHO_CLIENT_H__
#define __ECHO_CLIENT_H__

//Gestion du signal sigpipe
void sigpipeHandler(int signum);

//Gestion des fichiers ouverture/fermeture
void gestionDeFichier(char *nomTemp, headerClient *hc);

//Renommage des fichers (.dl)
int renommageFichier(char **nomFichier, char *nomTemp, headerClient *hc);

//Gestion des commandes
int gestionDesCommandes(char *cmd, headerClient *hc);


#endif