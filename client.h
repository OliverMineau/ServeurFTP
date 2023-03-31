#ifndef __ECHO_CLIENT_H__
#define __ECHO_CLIENT_H__


void sigpipeHandler(int signum);
void gestionDeFichier(char *nomTemp, headerClient *hc);
int renommageFichier(char *nomFichier, char *nomTemp, headerClient *hc);
int gestionDesCommandes(char *cmd, headerClient *hc);


#endif