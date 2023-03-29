
#include "csapp.h"
#include "structures.h"
#include "client.h"
#include <time.h>


int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE];

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
    
    char commande[MAXLINE];

    int deco = 0;
    while (!deco && Fgets(buf, MAXLINE, stdin) != NULL) {
        strcpy(commande,buf);
        commande[strlen(commande)-1]='\0';
        
        printf("Commande envoyée.\n");

        Rio_writen(clientfd, buf, strlen(buf));
        
        clock_t debut_temps = clock(); 

        //Prendre le header
        int n;
        header hd;
        if((n=Rio_readn(clientfd, &hd, sizeof(hd))) <= 0){
            printf("Server has prematurely closed the connection %d \n",n);
            break;
        }

        //Gestion des erreurs
        switch (hd.flag)
        {
        case FLAG_ERR_FICH_INEX:
            printf("%s: Aucun fichier de ce type.\n", commande);
            continue;
            break;

        case FLAG_ERR_CMD_INC:
            printf("%s: Commande inconnue.\n", commande);
            continue;
            break;
        
        case FLAG_DISCONNECT:
            deco=1;
            continue;
            break;
        
        default:
            break;
        }

        int total_bytes = hd.taille;
        printf("Total : %d\n",total_bytes);

        char* nomfichier = strtok(commande, " ");
        nomfichier = strtok(NULL, " ");
        
        int f = open(nomfichier, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        int total_bytes_read = 0;
        bloc blc;
        while ((n = Rio_readn(clientfd, blc.data, 256)) > 0 && total_bytes_read<total_bytes)
        {
            if(total_bytes_read+n > total_bytes){
                int nn = total_bytes-total_bytes_read;
                Rio_writen(f, blc.data, nn);
                total_bytes_read += nn;

                #ifdef DEBUG
                printf("Lu %doctets\n",nn);
                #endif
                break;

            }else{
                Rio_writen(f, blc.data, n);
                total_bytes_read += n;

                #ifdef DEBUG
                printf("Lu %doctets\n",n);
                #endif
            }

            
        } 

        Close(f);

        clock_t fin_temps = clock();
        double duree = (double)(fin_temps - debut_temps) / CLOCKS_PER_SEC;
        int debit = (total_bytes_read/1000) / duree;
        printf("Reception de %d octets en %f secondes (%d Koctets/s)\n",total_bytes_read,duree,debit);
        
    }

    Close(clientfd);
    exit(0);
}
