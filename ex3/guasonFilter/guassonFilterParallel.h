#ifndef guassonFilter_P_h
#define guassonFilter_P_h

#include "guassonFilter.h"

// Define an RGBA pixel structure
// typedef struct {
//     unsigned char r, g, b, a;
// } RGBA;

// // Define an Image structure
// typedef struct {
//     int width;
//     int height;
//     RGBA *pixels;
// } Image;

// Image *loadImage(const char *filename);

// void saveImage(const char *filename, Image *image);

void createGaussianKernel_P(int radius, double sigma, double **kernel, double *sum);

Image *createBlurredImage_P(int radius, Image *image);

#endif /* guassonFilter_h */
