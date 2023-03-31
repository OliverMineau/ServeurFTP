#ifndef __ECHO_SERVERI_H__
#define __ECHO_SERVERI_H__


int lireFichier(headerClient hc, int connfd);
int lireCommande(int connfd, char *client_name);


#endif