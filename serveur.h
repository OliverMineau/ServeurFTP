#ifndef __ECHO_SERVERI_H__
#define __ECHO_SERVERI_H__

//Ouverture et envoi des données du fichier
int lireFichier(headerClient hc, int connfd);

//Gestion des commandes
int lireCommande(int connfd, char *client_name);


#endif