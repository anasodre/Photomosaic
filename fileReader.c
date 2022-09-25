#include "fileReader.h"

// Ignora os comentarios enquanto comecar com hashtag
void ignoreComments (FILE *arq, int *c) {
    while (*c == '#') {
        while ((getc(arq)) != '\n');
        *c = getc(arq);
    }
}

// Checa se a linha eh o comeco de um comentario
// e o ignora enquanto for comentario
void checkComments (FILE *arq) {
    int c;
    c = getc(arq);
    if (c == '#')
        ignoreComments(arq,&c);
    ungetc(c, arq);

    // Pula a leitura do newline
    if (c == '\n')
        c = getc(arq);
}

// Le o formato do arquivo
char *readFormat (FILE *arq) {
    char formatReaded[3];
    checkComments(arq);

    // Le as duas primeiras letras e verifica se a leitura deu certo
    if (fscanf(arq, "%2s ", formatReaded) != 1) {
        perror ("Erro na leitura do formato");
        exit (1); 
    }
    // Se der certo, verifica se eh um dos formatos possiveis
    if (strcmp(formatReaded, "P6") && strcmp(formatReaded,"P3")) {
            perror ("Não está no formato P6");
            exit (1);
        }

    // Retorna o formato da imagem
    return (strdup(formatReaded));

}

// Le um inteiro e o retorna
int readInt (FILE *arq) {
    int num, ret;

    // Ignora os comentarios, se houver
    checkComments(arq);

    // Le o inteiro e o guarda na variavel num
    ret = fscanf(arq, "%d", &num);

    // Verifica se a leitura deu certo
    if (ret <= 0) {
        perror ("Erro na leitura de inteiro");
        exit (1);
    }

    // Retorna o inteiro lido
    return num;

}

// Le os pixels da pastilha ou imagem
void readPixel (char *type, unsigned char *colorOut, FILE *arq) {
    
    // Se for P6, usa fread para ler
    if (!strcmp(type,"P6")) {
        unsigned char color;

        fread(&color, sizeof(char),1,arq);        
        *colorOut = color;
    }

    // Se for P3, usa fscanf e converte o tipo do conteudo lido
    // de int para unsigned char
    else {
        int color;
        fscanf(arq, "%d ", &color);
        *colorOut = (unsigned char) color;
    }
}

// Guarda os pixels do arquivo PPM em uma matriz
void fillPixels (tImage **image, FILE *arq) {
    int i, j, num;
    unsigned char r, g, b; 
    int sum_r = 0, sum_g = 0, sum_b = 0;

    // Verifica se ha comentarios
    checkComments(arq);

    // Calcula o numero de pixels
    num = ((*image)->width)*((*image)->height);

    // Le os valores RGB e os armazena na matriz
    for (i = 0; i < (*image)->height; i++) {
        for (j = 0; j < (*image)->width; j++) {

            readPixel((*image)->format, &r, arq);
            readPixel((*image)->format, &g, arq);
            readPixel((*image)->format, &b, arq);
            
            (*image)->pixel[i][j].r = r;
            (*image)->pixel[i][j].g = g;
            (*image)->pixel[i][j].b = b;
            
            sum_r += r;
            sum_g += g;
            sum_b += b;
           
        }
    }

    // Calcula a cor media da imagem e guarda o valor
    (*image)->meanR = sum_r/num;
    (*image)->meanG = sum_g/num;
    (*image)->meanB = sum_b/num;
}