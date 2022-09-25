#ifndef __FOTOMOSAICO__
#define __FOTOMOSAICO__

#include "fileReader.h"

// Aloca uma matriz do tipo tPixels
tPixel **allocateMatrix (int w, int h) ;

// Cria um nodo na lista para armazenar cada imagem lida
tImage *createNodo (FILE *arq);

// Armazena as pastilhas na estrutura tImage
void storePPM (FILE *arq, tImage **nodo);

// Le as pastilhas do diretorio e as armazena em uma lista tImage
tImage *readPPM (char *pathTiles);

// Remove todos os nodos da lista
void removeList (tImage *lista);

// Le a imagem que sera transformada em fotomosaico
tImage *readImage();

// Retorna a pastilha mais adequada para substituir na imagem
tImage *newBlock (tImage *tiles, unsigned char meanR, unsigned char meanG, unsigned char meanB);

// Substitui o bloco dentro da imagem pela pastilha mais adequada
void replaceBlock (tImage **image, tImage *tiles, int iniI, int iniJ, int maxW, int maxH);

// Cria a imagem de saida
void createImage (tImage **image, tImage *tiles);

// Cria o arquivo para a imagem de saida
void createPPMfile (tImage *image);

#endif