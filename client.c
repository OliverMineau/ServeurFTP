
#include "csapp.h"
#include "structures.h"
#include "client.h"
#include <time.h>



void sigpipeHandler(int signum){
    printf("Sigpipe handler\n");
}

int main(int argc, char **argv)
{

    Signal(SIGPIPE,sigpipeHandler);

    int clientfd, port;
    char *host;

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
    
    char commande[MAX_CMD];

    int deco = 0;
    while (!deco && Fgets(commande, MAX_CMD, stdin) != NULL) {
        commande[strlen(commande)-1]='\0';
        
        char *cmd = strtok(commande, " ");
        char *nomfichier = strtok(NULL, " ");
        
        int cmd_num;
        if(!strcmp(cmd,"get")){
            cmd_num=CMD_GET;
            Rio_writen(clientfd,&cmd_num, sizeof(int));
            //Envoi nom fichier
            Rio_writen(clientfd, nomfichier, MAX_CMD);

        }else if(!strcmp(cmd,"bye")){
            cmd_num=CMD_BYE;
            Rio_writen(clientfd,&cmd_num, sizeof(int));

        }else{
            printf("Commande inconnue\n");
            continue;
        }

        
        
        printf("Commande envoyée.\n");



        clock_t debut_temps = clock(); 

        //Prendre le header
        int n;
        header hd;
        if((n=Rio_readn(clientfd, &hd, sizeof(hd))) < 0){
            printf("Server has prematurely closed the connection %d \n",n);
            break;
        }

        if(n==0){
            printf("Serveur fermé\n");
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
        
        default:
            break;
        }

        int total_bytes = hd.taille;
        printf("Total : %d\n",total_bytes);


        int f = open(nomfichier, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        int total_bytes_read = 0;
        bloc blc;
        while (total_bytes_read<total_bytes && (n = Rio_readn(clientfd, blc.data, 256)) > 0 )
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

        //Gestion erreur lecture pipe
        if(n == 0 && total_bytes_read < total_bytes){
            printf("Erreur Sigpipe Lecture\n");
            exit(1);
        }

        clock_t fin_temps = clock();
        double duree = (double)(fin_temps - debut_temps) / CLOCKS_PER_SEC;
        int debit = (total_bytes_read/1000) / duree;
        printf("Reception de %d octets en %f secondes (%d Koctets/s)\n",total_bytes_read,duree,debit);
        
    }

    Close(clientfd);
    exit(0);
}
