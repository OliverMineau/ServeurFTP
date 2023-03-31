#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#define MAX_CMD 100

#define FLAG_NO_ERR 0
#define FLAG_ERR_FICH_INEX 1
#define FLAG_DISCONNECT 2

#define CMD_GET 1
#define CMD_BYE 2


typedef struct {
    int flag;
    int taille;
} header;

typedef struct {
    int commande;
    char nomfichier[MAX_CMD];
    int position;
} headerClient;

typedef struct {
    char data[256];
} bloc;

#endif