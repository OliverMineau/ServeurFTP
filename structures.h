#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#define MAX_CMD 100

#define FLAG_NO_ERR 0
#define FLAG_ERR_FICH_INEX 1
#define FLAG_ERR_CMD_INC 2
#define FLAG_DISCONNECT 3

#define CMD_GET 0
#define CMD_BYE 1


typedef struct {
    int flag;
    int taille;
} header;

typedef struct {
    char data[256];
} bloc;

#endif