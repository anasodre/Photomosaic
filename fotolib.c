#include "fotolib.h"

// Aloca uma matriz do tipo tPixels
tPixel **allocateMatrix (int w, int h) {
    int i;
    tPixel **pixel = (tPixel **)malloc((h) * sizeof(tPixel*));

    // Verifica se a alocacao deu certo
    if (!pixel) {
        perror ("Erro: Memória insuficiente");
        exit(1);
    }
    for(i = 0; i < h; i++) 
        pixel[i] = (tPixel *)malloc((w) * sizeof(tPixel));
    
    // Retorna o endereco alocado
    return pixel;
}

// Cria um nodo na lista para armazenar cada imagem lida
tImage *createNodo (FILE *arq) {

    // Aloca o nodo tipo tImage
    tImage *image = NULL;
    image = (tImage *)malloc(sizeof(tImage));

    // Verifica se a alocacao deu certo
    if (!image) {
        perror ("Erro: Memória insuficiente");
        exit (1);
    }

    // Le o formato da imagem
    strcpy(image->format,readFormat(arq));
    
    // Le a largura, altura e valor maximo
    image->width = readInt(arq);
    image->height = readInt(arq);
    image->max = readInt(arq);
    
    // Aloca a matriz de pixels da imagem
    image->pixel = allocateMatrix(image->width, image->height);
    
    // Aponta para null a proxima imagem a ser lida
    image->prox = NULL;
    
    // Preenche a matriz de pixels
    fillPixels(&image, arq);

    return image;

}

// Armazena as pastilhas na estrutura tImage
void storePPM (FILE *arq, tImage **nodo) {
    tImage *aux;
    aux = *nodo;
    
    // Se nao houver elemento criado, cria o primeiro
    if (*nodo == NULL) {
        *nodo = createNodo(arq);
        return;
    }
    
    // Adiciona o novo nodo no final
    while (aux->prox != NULL) {
        aux = aux->prox;
    }

    aux->prox = createNodo(arq);

}

// Le as pastilhas do diretorio e as armazena em uma lista tImage
tImage *readPPM (char *pathTiles) {
    
    // Abre o diretorio
    struct dirent *direntry;
    DIR *tilesFolder = opendir(pathTiles);
    
    // Verifica se a abertura do diretorio deu certo
    if (tilesFolder == NULL) { 
        perror("Erro ao abrir o diretório das pastilhas");
        return 0; 
    }
    
    fprintf(stderr, "Reading tiles from %s\n", pathTiles);
    FILE* arq;

    tImage *inicio = NULL;
    int i = 0;

    char caminho[PATH_MAX];
    strcat(pathTiles, "/");
    
    // Le todos os arquivos do diretorio
    while ((direntry = readdir(tilesFolder)) != NULL) {
        
        // Verifica se o que foi lido eh um arquivo
        if (direntry->d_type == DT_REG) {
            strcpy(caminho,pathTiles);
            strcat(caminho, direntry->d_name);
            arq = fopen(caminho, "rb");

            // Verifica se o arquivo foi aberto
            if (arq) {

                // Armazena as informacoes da imagem
                storePPM (arq, &inicio);
                i++;
                fclose(arq);
            }
            else {
                perror("Não foi possível abrir o arquivo");
                exit (0);
            }
        }
    }

    fprintf(stderr, "%d tiles read\n", i);
    fprintf(stderr, "Tile size is %dx%d\n", inicio->width, inicio->height);
    fprintf(stderr, "Calculating tiles' average colors\n");
    
    // Fecha o diretorio
    closedir(tilesFolder);

    return inicio;
}

// Remove todos os nodos da lista
void removeList (tImage *lista) {
    tImage *no, *ptr_aux;
    int i;

    no = lista;
    while (no != NULL) {
        ptr_aux = no;
        no = no->prox;

        // Libera o espaco alocado para a matriz
        for (i = 0; i < ptr_aux->height; i++){  
            free(ptr_aux->pixel[i]);  
        }     
        free(ptr_aux->pixel);

        // Libera o espaco alocado para a lista tImage
        free(ptr_aux);
    }
    free(no);
}

// Le a imagem que sera transformada em fotomosaico
tImage *readImage() {
    tImage *image = NULL;

    // Cria um novo nodo para guardar as informacoes
    fprintf(stderr, "Reading input image\n");
    image = createNodo(stdin);

    fprintf(stderr, "Input image is PPM %s, %dx%d pixels\n", image->format, image->width, image->height);
    
    return image;
}

// Retorna a pastilha mais adequada para substituir na imagem
tImage *newBlock (tImage *tiles, unsigned char meanR, unsigned char meanG, unsigned char meanB) {
    float distance, menor;
    int distR, distG, distB, r;
    tImage *aux, *bloco;
    aux = tiles;

    // Calcula a cor predominante mais proxima a do bloco lido
    distR = aux->meanR - meanR;
    distG = aux->meanG - meanG;
    distB = aux->meanB - meanB;
    r = aux->meanR + meanR;

    // Usa o redmean para calcular a distancia entre as cores predominantes
    distance = sqrt((2+r/256)*distR*distR+4*distG*distG+(2+(255-r)/256)*distB*distB);
    menor = distance;
    bloco = aux;
    aux = aux->prox;

    while (aux != NULL) {
        distR = aux->meanR - meanR;
        distG = aux->meanG - meanG;
        distB = aux->meanB - meanB;
        r = aux->meanR + meanR;

        // Calcula a distancia
        distance = sqrt((2+r/256)*distR*distR+4*distG*distG+(2+(255-r)/256)*distB*distB);

        // Compara se eh o menor valor ja achado
        if (distance < menor) {
            menor = distance;
            bloco = aux;
        }
        aux = aux->prox;
    }
    
    // Retorna a pastilha mais adequada para substituir
    return bloco;
}

// Substitui o bloco dentro da imagem pela pastilha mais adequada
void replaceBlock (tImage **image, tImage *tiles, int iniI, int iniJ, int maxW, int maxH) {
    int i, j, num, n = 0, m;
    unsigned char meanR, meanB, meanG;
    int sum_r = 0, sum_g = 0, sum_b = 0;
    tImage *new;

    // Numero total de pixels
    num = (tiles->width)*(tiles->height);
    
    // Calcula o valor medio do bloco da imagem
    for (i = iniI; i < maxH; i++) {
        for (j = iniJ; j < maxW; j++) {
            sum_r += (*image)->pixel[i][j].r;
            sum_g += (*image)->pixel[i][j].g;
            sum_b += (*image)->pixel[i][j].b;
        }
    }

    // Calcula a media
    meanR = sum_r/num;
    meanG = sum_g/num;
    meanB = sum_b/num;
    
    // Recebe a pastilha mais adequada para substituir pelo bloco
    new = newBlock(tiles, meanR, meanG, meanB);

    // Troca os pixels da imagem pela da pastilha mais adequada
    for (i = iniI; i < maxH; i++) {
        m = 0;
        for (j = iniJ; j < maxW; j++) {
            (*image)->pixel[i][j].r = new->pixel[n][m].r;
            (*image)->pixel[i][j].g = new->pixel[n][m].g;
            (*image)->pixel[i][j].b = new->pixel[n][m++].b;
        }
        n++;
    }

} 

// Cria a imagem de saida
void createImage (tImage **image, tImage *tiles) {
    //Comparar cada bloco com cada pastilha e substituir
    tImage *auxTiles;
    auxTiles = tiles;
    int i, j, maxW, maxH;

    // Guarda os valores de largura e altura da imagem
    maxW = auxTiles->width;
    maxH = auxTiles->height;
    
    fprintf(stderr, "Building mosaic image\n");
    // Percorre a imagem por blocos
    for (i = 0; i < (*image)->height; i += auxTiles->height) {
        maxW = auxTiles->width;
        for (j = 0; j < (*image)->width; j += auxTiles->width) {
            if (maxW > (*image)->width)
                maxW = (*image)->width;
            replaceBlock(image,tiles,i,j,maxW,maxH);
            maxW += auxTiles->width;
        }
        maxH += auxTiles->height;
        if (maxH > (*image)->height)
            maxH = (*image)->height;
    }
    
    
}

// Cria o arquivo para a imagem de saida
void createPPMfile (tImage *image) {
    int i, j;
    
    fprintf(stderr, "Writing output file\n");
    // Escreve o formato, a largura e a altura
    fprintf(stdout,"%s\n",image->format);
    fprintf(stdout,"%i %i\n",image->width, image->height);
    fprintf(stdout,"%i\n",image->max);
    
    // Escreve o formato
    if (!strcmp(image->format,"P6")) {
        // Se for P6, usa fwrite para escrever o unsined char
        for (i = 0; i < image->height; i++) {
            for (j = 0; j < image->width; j++) {
                fwrite(&(image->pixel[i][j].r), sizeof(image->pixel[i][j].r), 1, stdout);
                fwrite(&(image->pixel[i][j].g), sizeof(image->pixel[i][j].g), 1, stdout);
                fwrite(&(image->pixel[i][j].b), sizeof(image->pixel[i][j].b), 1, stdout);
                
            }
        }
    }
    else {
        // Se for P3, converte de unsigned char para inteiro e
        // o escreve com fprintf
        for (i = 0; i < image->height; i++) {
            for (j = 0; j < image->width; j++) {
                int intR, intG, intB;
                intR = (int) image->pixel[i][j].r;
                intG = (int) image->pixel[i][j].g;
                intB = (int) image->pixel[i][j].b;
                fprintf(stdout, "%d %d %d ", intR, intG, intB);
            }
        }
    }
}