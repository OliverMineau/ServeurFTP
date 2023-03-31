
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
        
        char *cmd = strtok(commande, " \n");

        headerClient hc;

        if(cmd==NULL){
            continue;
        }
        
        //Gestion des commandes
        if(!strcmp(cmd,"get")){
            hc.commande=CMD_GET;

        }else if(!strcmp(cmd,"bye")){
            hc.commande=CMD_BYE;

        }else{
            printf("Commande inconnue\n");
            continue;
        }

        char *nomfichier = NULL;
        char newname[MAX_CMD];
        if(hc.commande!=CMD_BYE){
            //renommage nom du fichier en telechargement
            nomfichier = strtok(NULL, " \n");
            strcpy(newname,nomfichier);
            strcat(newname,".dl");

            hc.position = 0;
            strcpy(hc.nomfichier,nomfichier);

        }

        //si fichier existe pas Stat du fichier (taille)
        struct stat stat_f;
        if (hc.commande!=CMD_BYE) {
            
            if(stat(newname, &stat_f) >= 0){
                printf("Voulez vous reprendre le dernier telechargement ? (y/n) :");
                
                char rep[20];
                Fgets(rep, MAX_CMD, stdin);

                //Envoyer la position ou on est
                if(rep == NULL || rep[0] == 'y'){
                    hc.position = stat_f.st_size;
                }

                printf("Reprise du téléchargement de %s\n",hc.nomfichier);

            }else{
                printf("Téléchargement de %s\n",hc.nomfichier);
            }
            

        }

        if(hc.commande != CMD_BYE){
        }
        
        Rio_writen(clientfd, &hc, sizeof(headerClient));


        #ifdef DEBUG
            printf("Commande envoyée.\n");
        #endif


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


        #ifdef DEBUG
            printf("Total : %d\n",total_bytes);
        #endif

        //Overture du fichier
        int f;
        if(hc.position==0)
            f = open(newname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else
            f = open(newname, O_WRONLY | O_APPEND | O_CREAT, 0644);

        int total_bytes_read = 0;
        bloc blc;
        while (total_bytes_read<total_bytes && (n = Rio_readn(clientfd, blc.data, 256)) > 0 )
        {
            //printf("bien recu\n");

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
            printf("lu : %d\n",total_bytes_read);
            printf("total : %d\n",total_bytes);
            printf("n : %d\n",n);
            printf("Erreur Sigpipe Lecture\n");
            exit(1);
        }

        if(total_bytes_read == total_bytes && rename(newname, nomfichier) != 0) {
            fprintf(stderr,"Erreur: renommage fichier\n");
        }

        clock_t fin_temps = clock();
        double duree = (double)(fin_temps - debut_temps) / CLOCKS_PER_SEC;
        int debit = (total_bytes_read/1000) / duree;
        printf("Reception de %d octets en %f secondes (%d Koctets/s)\n\n",total_bytes_read,duree,debit);
        
    }

    Close(clientfd);
    exit(0);
}


/**
 * Erreur apres la reprise de telechargement err sigpipe serveur
 * 
 */