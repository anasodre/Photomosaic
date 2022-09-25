#ifndef __FILEREADERLIB__
#define __FILEREADERLIB__

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

struct tPixel {
    unsigned char r, g, b;
};
typedef struct tPixel tPixel;

struct tImage {
    char format[3];
    int width, height, max;
    unsigned char meanR, meanG, meanB;
    tPixel **pixel;
    struct tImage *prox;
};
typedef struct tImage tImage;

// Ignora os comentarios enquanto comecar com hashtag
void ignoreComments (FILE *arq, int *c);

// Checa se a linha eh o comeco de um comentario
// e o ignora enquanto for comentario
void checkComments (FILE *arq);

// Le o formato do arquivo
char *readFormat (FILE *arq);

// Le um inteiro e o retorna
int readInt (FILE *arq);

// Le os pixels da pastilha ou imagem
void readPixel (char *type, unsigned char *colorOut, FILE *arq);

// Guarda os pixels do arquivo PPM em uma matriz
void fillPixels (tImage **image, FILE *arq);

#endif