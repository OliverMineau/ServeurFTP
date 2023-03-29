#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#define FLAG_NO_ERR 0
#define FLAG_ERR_FICH_INEX 1
#define FLAG_ERR_CMD_INC 2
#define FLAG_DISCONNECT 3

typedef struct {
    int flag;
    int taille;
} header;

typedef struct {
    char data[256];
} bloc;

#endif