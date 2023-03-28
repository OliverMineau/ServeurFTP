/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

#define ERR_NO_ERR 0
#define ERR_FICH_INEX 1


int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);

    char nomfichier[MAXLINE];

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        strcpy(nomfichier,buf);
        nomfichier[strlen(nomfichier)-1]='\0';
        

        Rio_writen(clientfd, buf, strlen(buf));
        
        //Prendre code d'erreur
        int n;
        int erreur;
        if((n=Rio_readn(clientfd, &erreur, sizeof(int))) <= 0){
            printf("Server has prematurely closed the connection %d \n",n);
            break;
        }

        if(erreur==ERR_FICH_INEX){
            printf("%s: Aucun fichier de ce type\n", nomfichier);
            break;
        }


        //Prendre taille à lire
        int total_bytes;
        if((n=Rio_readn(clientfd, &total_bytes, sizeof(int))) <= 0){
            printf("Server has prematurely closed the connection %d \n",n);
            break;
        }
        printf("Total : %d\n",total_bytes);


        int f = open(nomfichier, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        int total_bytes_read = 0;
        while ((n = Rio_readlineb(&rio, buf, MAXLINE)) > 0 && total_bytes_read<total_bytes)
        {
            if(total_bytes_read+n > total_bytes){
                int nn = total_bytes-total_bytes_read;
                Rio_writen(f, buf, nn);
                total_bytes_read += nn;

            }else{
                Rio_writen(f, buf, n);
                total_bytes_read += n;
            }
        } 

        Close(f);

        printf("Reception de %d octets\n",total_bytes_read);
        
        break;
    }
    Close(clientfd);
    exit(0);
}
