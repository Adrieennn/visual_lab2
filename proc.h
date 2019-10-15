#ifndef __PROC_H
#define __PROC_H

void binomialFilter(gray* graymap, int rows, int cols, int maxval, int pgmraw);
void medianFilter(gray* graymap, int rows, int cols);
void histogram_building(gray* graymap, int length, int* hist);
void stretch_histogram(gray* graymap, int min, int max, int maxval, int len);
void equalize_histogram(gray* graymap, int* histogram, int maxval, int len);

#endif
