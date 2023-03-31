/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NB_SOCKET 1

#define MAX_NAME_LEN 256

int procPID[MAX_NB_SOCKET];


void echo(int connfd);

void childHandler(int signum){
	int pid;
	/*Tant qu'un processus fils est mort*/
	while((pid=waitpid(-1,NULL,WNOHANG|WUNTRACED))>0){
	}
}

void ctrlCHandler(int signum){
    for(int i = 0; i < MAX_NB_SOCKET; i++){
        Kill(procPID[i],SIGINT);
    }
    exit(0);
}

void lireFichier(int connfd)
{
    char buf[MAXLINE];
    rio_t rio;
   
    int cmdFd[2];
    if(pipe(cmdFd)==-1){
        fprintf(stderr,"Erreur creation pipe\n");
        exit(0);
    }

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);

    printf("Commande recu : %s",buf);

    int pid;
    if((pid=Fork())==0){
        
        Close(cmdFd[0]);
        dup2(cmdFd[1],STDOUT_FILENO);
        dup2(cmdFd[1],STDERR_FILENO);
        Close(STDIN_FILENO);

        for (int i = 0; i < MAXLINE; i++)
        {
            if(buf[i]=='\n'){
                buf[i]='\0';
                break;
            }
        }
        
        char *args[3];
        args[0]="cat";
        args[1]=buf;
        args[2]=NULL;
        execvp("cat",args);
        
        exit(0);

    }else{
                
        size_t n;
        char bufFichier[MAXLINE];
        rio_t rioFichier;

        Close(cmdFd[1]);
        Rio_readinitb(&rioFichier, cmdFd[0]);
        while ((n = Rio_readlineb(&rioFichier, bufFichier, MAXLINE)) != 0) {
            Rio_writen(connfd, bufFichier, n);
        }
        Close(cmdFd[0]);
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
    for(int i = 0; i < MAX_NB_SOCKET; i++){
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

