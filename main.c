#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Util.h"
#include "proc.h"
#define UPT 40
#define LOWT 20

int rows, cols;

float** scharr(gray* graymap, int rows, int cols) {
  int i, j, k, l;

  float xgrad[3][3];

  float constant = 16;
  xgrad[0][0] = -3;
  xgrad[0][1] = 0;
  xgrad[0][2] = 3;
  xgrad[1][0] = -10;
  xgrad[1][1] = 0;
  xgrad[1][2] = 10;
  xgrad[2][0] = -3;
  xgrad[2][1] = 0;
  xgrad[2][2] = 3;

  float ygrad[3][3];
  ygrad[0][0] = -3;
  ygrad[0][1] = -10;
  ygrad[0][2] = -3;
  ygrad[1][0] = 0;
  ygrad[1][1] = 0;
  ygrad[1][2] = 0;
  ygrad[2][0] = 3;
  ygrad[2][1] = 10;
  ygrad[2][2] = 3;

  float sumx, sumy;
  int xval, yval;

  float** res = malloc(2 * sizeof(float*));
  float* graymapx = malloc(cols * rows * sizeof(float));
  float* graymapy = malloc(cols * rows * sizeof(float));

  for (i = 1; i < rows - 1; i++) {
    for (j = 1; j < cols - 1; j++) {
      sumx = 0;
      sumy = 0;

      for (k = -1; k <= 1; k++) {
        for (l = -1; l <= 1; l++) {
          xval = i + k;
          yval = j + l;
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

          sumx += xgrad[k + 1][l + 1] * (float)graymap[xval * cols + yval];
          sumy += ygrad[k + 1][l + 1] * (float)graymap[xval * cols + yval];
        }
      }

      sumx /= constant;
      sumy /= constant;

      graymapx[i * cols + j] = sumx;
      graymapy[i * cols + j] = sumy;
    }
  }
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

int* nms(float** grads, int* mag, int rows, int cols) {
  int length = rows * cols;
  int i, j, *res = malloc(length * sizeof(int));
  int p1, p2;
  float dir;
  int temp, aa, ab, ag;

  for (i = 1; i < rows - 1; i++) {
    for (j = 1; j < cols - 1; j++) {
      dir = atan2(grads[1][i * cols + j], grads[0][i * cols + j]) * 4 / M_PI;
      temp = (int)roundf(dir);
      switch (abs(temp)) {
        case -4:
          p1 = i * cols + j + 1;
          p2 = i * cols + j - 1;
          break;
        case -3:
          p1 = i * (cols - 1) + j + 1;
          p2 = i * (cols + 1) + j - 1;
          break;
        case -2:
          p1 = i * (cols - 1) + j;
          p2 = i * (cols + 1) + j;
          break;
        case -1:
          p1 = i * (cols - 1) + j - 1;
          p2 = i * (cols + 1) + j + 1;
          break;
        case 0:
          p1 = i * cols + j - 1;
          p2 = i * cols + j + 1;
          break;
        case 1:
          p1 = i * (cols + 1) + j - 1;
          p2 = i * (cols - 1) + j + 1;
          break;
        case 2:
          p1 = i * (cols + 1) + j;
          p2 = i * (cols - 1) + j;
          break;
        case 3:
          p1 = i * (cols + 1) + j + 1;
          p2 = i * (cols - 1) + j - 1;
          break;
        default:
          p1 = i * cols + j + 1;
          p2 = i * cols + j - 1;
          break;
      }

      aa = abs(mag[p1]);
      ab = abs(mag[p2]);
      ag = abs(mag[i * cols + j]);
      if (aa < ag && ag >= ab) {
        res[i * cols + j] = mag[i * cols + j];
      } else {
        res[i * cols + j] = 0;
      }
    }
  }
  return res;
}

int* hyst(int* mag, int rows, int cols, int max) {
  int length = rows * cols, temp;
  int i, j, *res = malloc(length * sizeof(int));

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      temp = mag[i * cols + j];
      if (temp < LOWT) {
        res[i * cols + j] = 0;
      }
      if (temp >= UPT) {
        res[i * cols + j] = max;
      } else {
        res[i * cols + j] = temp;
      }
    }
  }

  for (i = 0; i < rows - 1; i++) {
    for (j = 0; j < cols - 1; j++) {
      if (res[i * cols + j] >= UPT) {
        // if bottom neighbor is above LOWT
        if (res[i * (cols + 1) + j] > LOWT) res[i * (cols + 1) + j] = max;
        // if right is above LOWT
        if (res[i * cols + j + 1] > LOWT) res[i * cols + j + 1] = max;
      }
    }
  }

  for (i = rows - 1; i > 0; i--) {
    for (j = cols - 1; j > 0; j--) {
      if (res[i * cols + j] >= UPT) {
        // if top neighbor is above LOWT
        if (res[i * (cols - 1) + j] > LOWT) res[i * (cols - 1) + j] = max;
        // if left neighbor is above LOWT
        if (res[i * cols + j - 1] > LOWT) res[i * cols + j - 1] = max;
      }
    }
  }

  for (i = 0; i < length; i++) {
    if (res[i] > 0 && res[i] < 255) res[i] = 0;
    // printf("%d\n", max);
  }
  return res;
}

int main(int argc, char* argv[]) {
  FILE* ifp;
  gray* graymap;
  int ich1, ich2, maxval = 255, pgmraw;
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

  int newmax = (int)((float)maxval * sqrtf(2));

  printf("%d %d \n", cols, rows);
  printf("%d\n", newmax);

  binomialFilter(graymap, rows, cols, maxval, pgmraw);
  float** grads = scharr(graymap, rows, cols);
  int* mag_img = magnitude(grads, rows * cols);
  int* nms_img = nms(grads, mag_img, rows, cols);
  int* hys_img = hyst(nms_img, rows, cols, newmax);

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      printf("%d ", hys_img[i * cols + j]);
    }
  }

  return 0;
}
