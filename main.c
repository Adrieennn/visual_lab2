#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Util.h"
#include "proc.h"

int rows, cols;


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
  printf("%d\n", maxval);

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
