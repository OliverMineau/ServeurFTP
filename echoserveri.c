/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define NB_PROC 4

#define MAX_NAME_LEN 256

#define ERR_NO_ERR 0
#define ERR_FICH_INEX 1

int procPID[NB_PROC];


void echo(int connfd);

void childHandler(int signum){
	int pid;
	/*Tant qu'un processus fils est mort*/
	while((pid=waitpid(-1,NULL,WNOHANG|WUNTRACED))>0){
	}
}

void ctrlCHandler(int signum){
    for(int i = 0; i < NB_PROC; i++){
        Kill(procPID[i],SIGINT);
    }
    exit(0);
}

void lireFichier(int connfd){
    char commande[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, commande, MAXLINE);

    //Enlever le \n a la fin de la commande
    commande[strlen(commande)-1]='\0';

    printf("Commande recu : %s\n",commande);

    size_t n;
    char bufFichier[MAXLINE];
    rio_t rioFichier;
    
    int f = open(commande,O_RDONLY);


    //Gestion de l'ouverture du fichier
    if (f != -1){

        struct stat stat_f;
        if (stat(commande, &stat_f) < 0) {
            return;
        }
        printf("Taille fichier : %ld\n",stat_f.st_size);


        Rio_readinitb(&rioFichier, f);
        int err = ERR_NO_ERR;

        //Envoi du code d'erreur 0;
        Rio_writen(connfd, &err, sizeof(int));
        Rio_writen(connfd, &stat_f.st_size, sizeof(int));
        while ((n = Rio_readlineb(&rioFichier, bufFichier, MAXLINE)) > 0) {
            Rio_writen(connfd, bufFichier, n);
        }

        Close(f);

    }else{
        int err = ERR_FICH_INEX;
        Rio_writen(connfd,&err, sizeof(int));
    }


}


/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    Signal(SIGCHLD,childHandler);
    Signal(SIGINT,ctrlCHandler);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    int pid;
    //Creation des processus
    for(int i = 0; i < NB_PROC; i++){
        if((pid=Fork())==0){
            Signal(SIGINT,SIG_DFL);
            break;

        }else{
            procPID[i]=pid;
        }
    }

    if(pid == 0) {
        //Fils
        
        while(1){

            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            
            if(connfd == -1){
                continue;
            }

            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);
            
            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                    INET_ADDRSTRLEN);
            
            printf("server connected to %s (%s) - fd:%d\n", client_hostname,
                client_ip_string,connfd);
            
            //Lire donnée du client
            lireFichier(connfd);

            Close(connfd);
        }

    }else{
        //Pere
        while(1){
            sleep(1);
        }
    }
}

