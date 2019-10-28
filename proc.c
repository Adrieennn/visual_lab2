#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Util.h"

#define KERNEL 1
#define N 2  // How often do we smooth the image.

#define UPT 60 // Upper threshold for hysteresis
#define LOWT 50 // Lower threshold for hysteresis

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

  for (i = 1; i < rows - 1; i++) {
    for (j = 1; j < cols - 1; j++) {
      dir = atan2(grads[1][i * cols + j], grads[0][i * cols + j]) * 4 / M_PI;
      switch ((int)roundf(dir)) {
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

      if (mag[p1] < mag[i * cols + j] && mag[i * cols + j] >= mag[p2]) {
        res[i * cols + j] = mag[i * cols + j];
      } else {
        res[i * cols + j] = 0;
      }
    }
  }
  return res;
}

float** scharr(gray* graymap, int rows, int cols) {
  int i, j, k, l;
  float constant = 16;
  float sumx, sumy;
  int xval, yval;

  float xgrad[3][3];
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
          } else if (xval >= rows) {
            xval = rows - 1;
          }
          if (yval < 0) {
            yval = 0;
          } else if (yval >= cols) {
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
  }
  return res;
}
