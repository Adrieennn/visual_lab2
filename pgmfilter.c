#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Util.h"

#define KERNEL 1
#define N 2  // How often do we smooth the image.

void binomialFilter(gray* graymap, int rows, int cols, int maxval, int pgmraw) {
  int i, j, k, l;

  int binomialFilter[1 + 2 * KERNEL][1 + 2 * KERNEL];
  int constant = 256;

  if (KERNEL == 1) {
    constant = 16;

    binomialFilter[0][0] = 1;
    binomialFilter[0][1] = 2;
    binomialFilter[0][2] = 1;
    binomialFilter[1][0] = 2;
    binomialFilter[1][1] = 4;
    binomialFilter[1][2] = 2;
    binomialFilter[2][0] = 1;
    binomialFilter[2][1] = 2;
    binomialFilter[2][2] = 1;
  } else if (KERNEL == 2) {
    constant = 256;

    binomialFilter[0][0] = 1;
    binomialFilter[0][1] = 4;
    binomialFilter[0][2] = 6;
    binomialFilter[0][3] = 4;
    binomialFilter[0][4] = 1;
    binomialFilter[1][0] = 4;
    binomialFilter[1][1] = 16;
    binomialFilter[1][2] = 24;
    binomialFilter[1][3] = 16;
    binomialFilter[1][4] = 4;
    binomialFilter[2][0] = 6;
    binomialFilter[2][1] = 24;
    binomialFilter[2][2] = 36;
    binomialFilter[2][3] = 24;
    binomialFilter[2][4] = 6;
    binomialFilter[3][0] = 4;
    binomialFilter[3][1] = 16;
    binomialFilter[3][2] = 24;
    binomialFilter[3][3] = 16;
    binomialFilter[3][4] = 4;
    binomialFilter[4][0] = 1;
    binomialFilter[4][1] = 4;
    binomialFilter[4][2] = 6;
    binomialFilter[4][3] = 4;
    binomialFilter[4][4] = 1;
  } else {
    exit(EXIT_FAILURE);
  }

  float sum;

  gray* graymapnew = (gray*)malloc(cols * rows * sizeof(gray));

  for (int n = 0; n < N; n++) {
    for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
        sum = 0;

        for (k = -KERNEL; k <= KERNEL; k++) {
          for (l = -KERNEL; l <= KERNEL; l++) {
            // pixel is inside the image
            if (i + k >= 0 && j + l >= 0 && i + k < rows && j + l < cols) {
              sum += binomialFilter[k + KERNEL][l + KERNEL] *
                     graymap[(i + k) * cols + j + l];

              // y coordinate is not inside the image, so no offset on j
            } else if (i + k >= 0 && i + k < rows &&
                       (j + l < 0 || j + l >= cols)) {
              sum += binomialFilter[k + KERNEL][l + KERNEL] *
                     graymap[(i + k) * cols + j];

              // x coordinate is not insdie the image, so no offset on i
            } else if (j + l >= 0 && j + l < cols &&
                       (i + k < 0 || i + k >= rows)) {
              sum += binomialFilter[k + KERNEL][l + KERNEL] *
                     graymap[i * cols + j + l];
            } else {  // pixel requested is beyond a corner
              if (i + k <= 0 && j + l <= 0) {  // top-left
                sum += binomialFilter[k + KERNEL][l + KERNEL] * graymap[0];

              } else if (i + k <= 0 && j + l >= cols) {  // top-right
                sum +=
                    binomialFilter[k + KERNEL][l + KERNEL] * graymap[cols - 1];

              } else if (i + k >= rows && j + l <= 0) {  // bottom-left
                sum += binomialFilter[k + KERNEL][l + KERNEL] *
                       graymap[(rows - 1) * cols];

              } else {  // bottom-right
                sum += binomialFilter[k + KERNEL][l + KERNEL] *
                       graymap[(rows - 1) * cols + rows - 1];
              }
            }
          }
        }
        sum /= (float)constant;
        sum = sum > maxval ? maxval : sum;
        // putchar((int)sum);
        graymapnew[i * cols + j] = (gray)sum;
      }
    }
    gray* temp = graymap;  // Swap both graymaps
    graymap = graymapnew;
    graymapnew = temp;
  }

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      putchar((int)graymap[i * cols + j]);
    }
  }
}

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

      for (k = -KERNEL; k <= KERNEL; k++) {
        for (l = -KERNEL; l <= KERNEL; l++) {
          // pixel is inside the image
          if (i + k >= 0 && j + l >= 0 && i + k < rows && j + l < cols) {
            sumx += binomialFilter[k + KERNEL][l + KERNEL] *
                    (float)graymap[(i + k) * cols + j + l];
            sumy += binomialFilter[l + KERNEL][k + KERNEL] *
                    (float)graymap[(i + k) * cols + j + l];

            // y coordinate is not inside the image, so no offset on j
          } else if (i + k >= 0 && i + k < rows &&
                     (j + l < 0 || j + l >= cols)) {
            sumx += binomialFilter[k + KERNEL][l + KERNEL] *
                    (float)graymap[(i + k) * cols + j];
            sumy += binomialFilter[l + KERNEL][k + KERNEL] *
                    (float)graymap[(i + k) * cols + j];

            // x coordinate is not insdie the image, so no offset on i
          } else if (j + l >= 0 && j + l < cols &&
                     (i + k < 0 || i + k >= rows)) {
            sumx += binomialFilter[k + KERNEL][l + KERNEL] *
                    (float)graymap[i * cols + j + l];

            sumy += binomialFilter[l + KERNEL][k + KERNEL] *
                    (float)graymap[i * cols + j + l];
          } else {  // pixel requested is beyond a corner
            if (i + k <= 0 && j + l <= 0) {  // top-left
              sumx +=
                  binomialFilter[k + KERNEL][l + KERNEL] * (float)graymap[0];
              sumy +=
                  binomialFilter[l + KERNEL][k + KERNEL] * (float)graymap[0];

            } else if (i + k <= 0 && j + l >= cols) {  // top-right
              sumx += binomialFilter[k + KERNEL][l + KERNEL] *
                      (float)graymap[cols - 1];
              sumy += binomialFilter[l + KERNEL][k + KERNEL] *
                      (float)graymap[cols - 1];

            } else if (i + k >= rows && j + l <= 0) {  // bottom-left
              sumx += binomialFilter[k + KERNEL][l + KERNEL] *
                      (float)graymap[(rows - 1) * cols];
              sumy += binomialFilter[l + KERNEL][k + KERNEL] *
                      (float)graymap[(rows - 1) * cols];

            } else {  // bottom-right
              sumx += binomialFilter[k + KERNEL][l + KERNEL] *
                      (float)graymap[(rows - 1) * cols + rows - 1];
              sumy += binomialFilter[l + KERNEL][k + KERNEL] *
                      (float)graymap[(rows - 1) * cols + rows - 1];
            }
          }
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

int cmpfnc(const void* a, const void* b) {
  int int_a = *((int*)a);
  int int_b = *((int*)b);

  if (int_a == int_b)
    return 0;
  else if (int_a < int_b)
    return -1;
  else
    return 1;
}

void medianFilter(gray* graymap, int rows, int cols) {
  int i, j, k, l, len_median;
  int off = 1 + 2 * KERNEL;
  int medianList[off * off];

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      len_median = 0;

      for (k = -KERNEL; k <= KERNEL; k++) {
        for (l = -KERNEL; l <= KERNEL; l++) {
          if (i + k >= 0 && l + j >= 0 && i + k < rows - KERNEL &&
              l + j < cols - KERNEL) {
            medianList[len_median] = graymap[(i + k) * cols + j + l];
            len_median++;
          }
        }
      }
      qsort(medianList, len_median, sizeof(int), cmpfnc);

      putchar(medianList[len_median / 2]);
    }
  }
}

void histogram_building(gray* graymap, int length, int* hist) {
  int i;

  for (i = 0; i < length; i++) {
    hist[graymap[i]]++;
  }
}

void stretch_histogram(gray* graymap, int min, int max, int maxval, int len) {
  int i, c;
  for (i = 0; i < len; i++) {
    c = (float)maxval * ((float)(graymap[i] - min) / (float)(max - min));
    putchar(c);
  }
}

void equalize_histogram(gray* graymap, int* histogram, int maxval, int len) {
  float nb_pixels = 0, numerator = 0, nums[maxval];
  int i, c;

  for (i = 0; i <= maxval; i++) {
    nb_pixels += (float)histogram[i];
    nums[i] = nb_pixels;
  }

  for (i = 0; i < len; i++, numerator = 0) {
    numerator = nums[graymap[i]];
    c = (float)maxval * (float)((float)numerator / (float)nb_pixels);
    putchar(c);
  }
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
  //printf("%d\n", maxval);
  printf("%d\n", (int)((float)maxval * sqrtf(2)));

  float** grads = scharr(graymap, rows, cols);
  int* img = magnitude(grads, rows * cols);

  for (i = 0; i < rows * cols; i++) {
    printf("%d ", ((int)img[i]));
  }

  return 0;
}
