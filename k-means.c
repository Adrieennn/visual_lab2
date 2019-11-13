#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Util.h"
#include "proc.h"

#define NB_CLUSTER 6
#define ITERATION 20
#define LAMBDA 0.1

typedef struct center {
  int r, g, b, x, y;
} center_t;

int rows, cols;

center_t *init_center() {
  /*initialize all the centers randomly*/
  center_t *k;
  k = (center_t *)malloc(NB_CLUSTER * sizeof(center_t));
  int i;

  for (i = 0; i < NB_CLUSTER; i++) {
    k[i].r = rand() % 255;
    k[i].g = rand() % 255;
    k[i].b = rand() % 255;
    k[i].x = rand() % cols;
    k[i].y = rand() % rows;
  }
  return k;
}

center_t *copy_center(center_t *c) {
  center_t *k;
  k = (center_t *)malloc(NB_CLUSTER * sizeof(center_t));
  int i;

  for (i = 0; i < NB_CLUSTER; i++) {
    k[i].r = c[i].r;
    k[i].g = c[i].g;
    k[i].b = c[i].b;
    k[i].x = c[i].x;
    k[i].y = c[i].y;
  }
  return k;
}

void allocation(gray **img, int *map, center_t *k) {
  /*allocate points to cluster center*/
  int i, j;
  int l, minIdx, minDistance, distance;

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      minIdx = -1;
      minDistance = 3 * 255 * 255;
      for (l = 0; l < NB_CLUSTER; l++) {
        distance =
            (img[0][i * cols + j] - k[l].r) * (img[0][i * cols + j] - k[l].r) +
            (img[1][i * cols + j] - k[l].g) * (img[0][i * cols + j] - k[l].g) +
            (img[2][i * cols + j] - k[l].b) * (img[0][i * cols + j] - k[l].b) +
            (int)(LAMBDA * ((i - k[l].x) * (i - k[l].x) +
                                   (j - k[l].y) * (j - k[l].y)));
        if (distance < minDistance) {
          minDistance = distance;
          minIdx = l;
        }
      }

      map[i * cols + j] = minIdx;
    }
  }
}

void recalculation(gray **img, int *map, center_t *k) {
  /*recaculate cluster center*/
  int i, j;
  int cluster[NB_CLUSTER][5];
  int nbPt[NB_CLUSTER];
  for (i = 0; i < NB_CLUSTER; i++) {
    nbPt[i] = 1;
    cluster[i][0] = 0;
    cluster[i][1] = 0;
    cluster[i][2] = 0;
    cluster[i][3] = 0;
    cluster[i][4] = 0;
  }

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      cluster[map[i * cols + j]][0] += img[0][i * cols + j];
      cluster[map[i * cols + j]][1] += img[1][i * cols + j];
      cluster[map[i * cols + j]][2] += img[2][i * cols + j];
      cluster[map[i * cols + j]][3] += i;
      cluster[map[i * cols + j]][4] += j;
      nbPt[map[i * cols + j]]++;
    }
  }

  for (i = 0; i < NB_CLUSTER; i++) {
    k[i].r = cluster[i][0] / nbPt[i];
    k[i].g = cluster[i][1] / nbPt[i];
    k[i].b = cluster[i][2] / nbPt[i];
    k[i].x = cluster[i][3] / nbPt[i];
    k[i].y = cluster[i][4] / nbPt[i];
  }
}

center_t *kmeans_while(gray **img, int *map) {
  int i, diff = 0;
  center_t *k = init_center();
  center_t *l = copy_center(k);

  while (!diff) {
    allocation(img, map, k);
    recalculation(img, map, k);

    diff = 1;
    for (i = 0; i < NB_CLUSTER && diff; i++) {
      if (k[i].x != l[i].x || k[i].y != l[i].y) {
        l = copy_center(k);
        diff = 0;
      }
    }
  }
  return k;
}

center_t *kmeans(gray **img, int *map) {
  int i;
  center_t *k = init_center();

  for (i = 0; i < ITERATION; i++) {
    allocation(img, map, k);
    recalculation(img, map, k);
  }
  return k;
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
  time_t t;
  srand((unsigned)time(&t));
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
  center_t *k = kmeans(img, map);

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
  return 0;
}

