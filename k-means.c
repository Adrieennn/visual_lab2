//
// Created by adrien on 04/11/2019.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Util.h"
#include "proc.h"

#define NB_CLUSTER 6
#define ITERATION 30

typedef struct center {
  int r, g, b, x, y;
} center_t;

int rows, cols;

void initialisation(center_t *k) {
  /*initialize all the centers randomly*/
  int i;
  for (i = 0; i < NB_CLUSTER; i++) {
    k[i].r = rand() % 255;
    k[i].g = rand() % 255;
    k[i].b = rand() % 255;
    k[i].x = rand() % cols;
    k[i].y = rand() % rows;
  }
}

void allocation(gray **img, int *map, center_t *k) {
  /*allocate points to cluster center*/
  int i, j;
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      int l, minIdx, minDistance;
      minIdx = -1;
      minDistance = 3 * 255 * 255;
      for (l = 0; l < NB_CLUSTER; l++) {
        int distance;
        distance =
            (img[0][i * cols + j] - k[l].r) * (img[0][i * cols + j] - k[l].r) +
            (img[1][i * cols + j] - k[l].g) * (img[0][i * cols + j] - k[l].g) +
            (img[2][i * cols + j] - k[l].b) * (img[0][i * cols + j] - k[l].b);
        if (distance < minDistance) {
          minDistance = distance;
          minIdx = l;
        }
      }

      map[i * cols + j] = minIdx;
    }
  }
}

void recaculation(gray **img, int *map, center_t *k) {
  /*recaculate cluster center*/
  int i, j;
  int cluster[NB_CLUSTER][3];
  int nbPt[NB_CLUSTER];
  int l;
  for (l = 0; l < NB_CLUSTER; l++) {
    nbPt[l] = 1;
    cluster[l][0] = 0;
    cluster[l][1] = 0;
    cluster[l][2] = 0;
  }

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      cluster[map[i * cols + j]][0] += img[0][i * cols + j];
      cluster[map[i * cols + j]][1] += img[1][i * cols + j];
      cluster[map[i * cols + j]][2] += img[2][i * cols + j];
      nbPt[map[i * cols + j]]++;
    }
  }

  for (l = 0; l < NB_CLUSTER; l++) {
    k[l].r = cluster[l][0] / nbPt[l];
    k[l].g = cluster[l][1] / nbPt[l];
    k[l].b = cluster[l][2] / nbPt[l];
  }
}

int main(int argc, char *argv[]) {
  FILE *ifp;
  gray *redmap, *greenmap, *bluemap;
  gray **img;
  int *map;
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
  img = malloc(3 * sizeof(gray *));
  redmap = (gray *)malloc(cols * rows * sizeof(gray));
  greenmap = (gray *)malloc(cols * rows * sizeof(gray));
  bluemap = (gray *)malloc(cols * rows * sizeof(gray));
  map = malloc(cols * rows * sizeof(int));

  /* Reading */
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      if (pgmraw) {
        redmap[i * cols + j] = pm_getrawbyte(ifp);
        greenmap[i * cols + j] = pm_getrawbyte(ifp);
        bluemap[i * cols + j] = pm_getrawbyte(ifp);
      } else {
        redmap[i * cols + j] = pm_getint(ifp);
        greenmap[i * cols + j] = pm_getint(ifp);
        bluemap[i * cols + j] = pm_getint(ifp);
      }
      map[i * cols + j] = -1;
    }
  }

  img[0] = redmap;
  img[1] = greenmap;
  img[2] = bluemap;

  /* Closing */
  fclose(ifp);

  /* Applying k-means */
  center_t *k;
  k = (center_t *)malloc(NB_CLUSTER * sizeof(center_t));
  initialisation(k);

  for (i = 0; i < ITERATION; i++) {
    allocation(img, map, k);
    recaculation(img, map, k);
  }

  /* Writing */
  if (pgmraw)
    printf("P6\n");
  else
    printf("P3\n");

  printf("%d %d \n", cols, rows);
  printf("%d\n", maxval);

  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      if (pgmraw)
        printf("%c%c%c", k[map[i * cols + j]].r, k[map[i * cols + j]].g,
               k[map[i * cols + j]].b);
      else
        printf("%d %d %d", k[map[i * cols + j]].r, k[map[i * cols + j]].g,
               k[map[i * cols + j]].b);
}

