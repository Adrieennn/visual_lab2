#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Util.h"
#include "proc.h"

float** scharr(gray* graymap, int rows, int cols) {
  int i, j, k, l;

  float binomialFilter[3][3];

  float constant = 16;
  binomialFilter[0][0] = -3;
  binomialFilter[0][1] = 0;
  binomialFilter[0][2] = 3;
  binomialFilter[1][0] = -10;
  binomialFilter[1][1] = 0;
  binomialFilter[1][2] = 10;
  binomialFilter[2][0] = -3;
  binomialFilter[2][1] = 0;
  binomialFilter[2][2] = 3;

  float sumx, sumy;

  float* graymapx = malloc(cols * rows * sizeof(float));
  float* graymapy = malloc(cols * rows * sizeof(float));

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      sumx = 0;
      sumy = 0;

      for (k = -1; k <= 1; k++) {
        for (l = -1; l <= 1L; l++) {
          int xval = i + k, yval = j + l;
          if (xval < 0) {
            xval = 0;
          } else if (xval > rows) {
            xval = rows - 1;
          }
          if (yval < 0) {
            yval = 0;
          } else if (yval > cols) {
            yval = cols - 1;
          }

          sumx +=
              binomialFilter[k + 1][l + 1] * (float)graymap[xval * cols + yval];
          sumy +=
              binomialFilter[l + 1][k + 1] * (float)graymap[xval * cols + yval];
        }
      }
      sumx /= constant;
      sumy /= constant;

      graymapx[i * cols + j] = sumx;
      graymapy[i * cols + j] = sumy;
    }
  }
  float** res = malloc(2 * sizeof(float*));
  res[0] = graymapx;
  res[1] = graymapy;
  return res;
}

int* magnitude(float** grads, int length) {
  int i;
  int* res = malloc(length * sizeof(int));
  for (i = 0; i < length; i++) {
    res[i] = (int)sqrtf(grads[0][i] * grads[0][i] + grads[1][i] * grads[1][i]);
  }
  return res;
}

int main(int argc, char* argv[]) {
  FILE* ifp;
  gray* graymap;
  int ich1, ich2, rows, cols, maxval = 255, pgmraw;
  int i, j;

  /* Arguments */
  if (argc != 2) {
    printf("\nUsage: %s file \n\n", argv[0]);
    exit(0);
  }

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
  if (ich2 != '2' && ich2 != '5')
    pm_erreur(" wrong file type ");
  else if (ich2 == '2')
    pgmraw = 0;
  else
    pgmraw = 1;

  /* Reading image dimensions */
  cols = pm_getint(ifp);
  rows = pm_getint(ifp);
  maxval = pm_getint(ifp);

  /* Memory allocation  */
  graymap = (gray*)malloc(cols * rows * sizeof(gray));

  /* Reading */
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      if (pgmraw)
        graymap[i * cols + j] = pm_getrawbyte(ifp);
      else
        graymap[i * cols + j] = pm_getint(ifp);

  /* Closing */
  fclose(ifp);

  /* Writing */
  if (pgmraw)
    printf("P2\n");
  else
    printf("P5\n");

  printf("%d %d \n", cols, rows);
  // printf("%d\n", maxval);
  printf("%d\n", (int)((float)maxval * sqrtf(2)));

  float** grads = scharr(graymap, rows, cols);
  int* img = magnitude(grads, rows * cols);

  for (i = 0; i < rows * cols; i++) {
    printf("%d ", img[i] > 120 ? img[i] : 0);
  }

  return 0;
}
