//
// Created by adrien on 04/11/2019.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Util.h"
#include "proc.h"

#define NB_CLUSTER 5

typedef struct center{
    int r,g,b,x,y;
}center_t;

int rows, cols;


void initialisation(center_t *k){
    /*initialize all the centers randomly*/
    int i;
    for(i = 0; i < NB_CLUSTER; i++){
        k[i].r = rand() % 255;
        k[i].g = rand() % 255;
        k[i].b = rand() % 255;
        k[i].x = rand() % cols;
        k[i].y = rand() % rows;
    }
}


void allocation(gray **img, int *map, center_t *k){
    /*allocate points to cluster center*/


}

void recaculation(gray **img, int *map, center_t *k){
    /*recaculate cluster center*/

}

int main(int argc, char* argv[]) {
    FILE *ifp;
    gray *redmap,*greenmap,*bluemap;
    gray **img;
    int* map;
    int ich1, ich2, maxval = 255, pgmraw;
    int i, j;
    /* Opening */
    ifp = fopen(argv[1], "r");
    if (ifp == NULL) {
        printf("error in opening file %s\n", argv[1]);
        exit(1);
    }

    /*  Magic number reading */
    ich1 = getc(ifp);
    if (ich1 == EOF) pm_erreur("EOF / read error / magic number");
    ich2 = getc(ifp);
    if (ich2 == EOF) pm_erreur("EOF /read error / magic number");
    if (ich2 != '3' && ich2 != '6')
        pm_erreur(" wrong file type ");
    else if (ich2 == '3')
        pgmraw = 0;
    else
        pgmraw = 1;

    /* Reading image dimensions */
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    maxval = pm_getint(ifp);

    /* Memory allocation  */
    img = malloc(3 * sizeof(gray*));
    redmap = (gray*)malloc(cols * rows * sizeof(gray));
    greenmap = (gray*)malloc(cols * rows * sizeof(gray));
    bluemap = (gray*)malloc(cols * rows * sizeof(gray));
    map = malloc(cols * rows * sizeof(int));

    /* Reading */
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){if (pgmraw){
                redmap[i * cols + j] = pm_getrawbyte(ifp);
                greenmap[i * cols + j] = pm_getrawbyte(ifp);
                bluemap[i * cols + j] = pm_getrawbyte(ifp);
            }
            else{
                redmap[i * cols + j] = pm_getint(ifp);
                greenmap[i * cols + j] = pm_getint(ifp);
                bluemap[i * cols + j] = pm_getint(ifp);
            }
        }
    }

    img[0]=redmap;
    img[1]=greenmap;
    img[2]=bluemap;


    /* Closing */
    fclose(ifp);


    /* Writing */
    if (pgmraw)
        printf("P2\n");
    else
        printf("P5\n");

    printf("%d %d \n", cols, rows);
    printf("%d\n",maxval);

    for(i=0; i < rows; i++)
        for(j=0; j < cols ; j++)
            if(pgmraw)
                printf("%d %d %d", redmap[i * cols + j],greenmap[i * cols + j],bluemap[i * cols + j]);
            else
                printf("%c%c%c",redmap[i * cols + j],greenmap[i * cols + j],bluemap[i * cols + j]);
}

