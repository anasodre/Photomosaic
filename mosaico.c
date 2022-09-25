#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fotolib.h"

int main (int argc, char *argv[]) {
    int option;
    char pathTiles[PATH_MAX];

    // Define, por default, o diretorio das pastilhas como o tiles
    strcpy(pathTiles, "./tiles");

    while ((option = getopt (argc, argv, "i:o:p:h")) != -1) {
        switch (option) {
          case 'i':      // seta a opcao -i e redireciona a leitura
            freopen(optarg,"r",stdin);
            break;
          case 'o':      // seta a opcao -o e redireciona a escrita
            freopen(optarg,"w",stdout);
            break;
          case 'p':      // seta a opcao -p e define a pasta das pastilhas
            strcpy(pathTiles, optarg);
            break;
          case 'h':
            fprintf(stderr, "%s é um programa que recebe uma imagem e um diretorio de pastilhas e cria um fotomosaico da imagem de entrada.\n", argv[0]);
            fprintf(stderr, "As flags são -i para imagem de entrada, -o para imagem de saída e -p diretorio das pastilhas\n");
            exit(0);
            break;
        }
    }
    
    // Le as pastilhas do diretorio
    tImage *tiles = NULL;
    tiles = readPPM(pathTiles);

    // Le a imagem que sera transformada em fotomosaico
    tImage *image = NULL;
    image = readImage();

    // Cria a imagem com o fotomosaico
    createImage(&image,tiles);

    // Prepara o arquivo de saida do fotomosaico
    createPPMfile(image);

    // Desaloca as listas criadas para guardar as imagens
    removeList(tiles);
    removeList(image);
    exit (0);
}