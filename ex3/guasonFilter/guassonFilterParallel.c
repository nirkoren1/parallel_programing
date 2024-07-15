#include "guassonFilterParallel.h"
// #include "guassonFilter.h"
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// // Function to load an image
// Image *loadImage(const char *filename) {
//     int width, height, channels;
//     unsigned char *data = stbi_load(filename, &width, &height, &channels, 4); // 4 means we want the image in RGBA format
//     if (data == NULL) {
//         printf("Failed to load image: %s\n", filename);
//         return NULL;
//     }

//     Image *image = (Image *)malloc(sizeof(Image));
//     image->width = width;
//     image->height = height;
//     image->pixels = (RGBA *)malloc(width * height * sizeof(RGBA));

//     for (int i = 0; i < width * height; i++) {
//         image->pixels[i].r = data[4 * i + 0];
//         image->pixels[i].g = data[4 * i + 1];
//         image->pixels[i].b = data[4 * i + 2];
//         image->pixels[i].a = data[4 * i + 3];
//     }

//     stbi_image_free(data);
//     return image;
// }

// // Function to save an image
// void saveImage(const char *filename, Image *image) {
//     unsigned char *data = (unsigned char *)malloc(image->width * image->height * 4);

//     for (int i = 0; i < image->width * image->height; i++) {
//         data[4 * i + 0] = image->pixels[i].r;
//         data[4 * i + 1] = image->pixels[i].g;
//         data[4 * i + 2] = image->pixels[i].b;
//         data[4 * i + 3] = image->pixels[i].a;
//     }

//     if (stbi_write_png(filename, image->width, image->height, 4, data, image->width * 4)) {
//         printf("Image saved successfully: %s\n", filename);
//     } else {
//         printf("Failed to save image: %s\n", filename);
//     }

//     free(data);
// }

void createGaussianKernel_P(int radius, double sigma, double **kernel, double *sum) {
    int kernelWidth = (2 * radius) + 1;
    *sum = 0.0;

    *kernel = (double *)malloc(kernelWidth * kernelWidth * sizeof(double));
    int x, y;

    // #pragma omp parallel for collapse(2) private(x, y) schedule(static)
    for (x = -radius; x <= radius; x++) {
        for (y = -radius; y <= radius; y++) {
            double exponentNumerator = -(x * x + y * y);
            double exponentDenominator = (2 * sigma * sigma);
            double eExpression = exp(exponentNumerator / exponentDenominator);
            double kernelValue = (eExpression / (2 * M_PI * sigma * sigma));
            (*kernel)[(x + radius) * kernelWidth + (y + radius)] = kernelValue;
            *sum += kernelValue;
        }
    }

    for (int i = 0; i < kernelWidth * kernelWidth; i++) {
        (*kernel)[i] /= *sum;
    }
}

void convolve_new(int width, int radius, int x, int y, Image* image, int kernelWidth, double *r, double *g, double *b, double sigma, double kernelSum) {
    int kernelX, kernelY;
    double redValue = 0.0;
    double greenValue = 0.0;
    double blueValue = 0.0;

    #pragma omp parallel for collapse(2) private(kernelX, kernelY) schedule(static) reduction(+: redValue, greenValue, blueValue)
    for (kernelX = -radius; kernelX <= radius; kernelX++) {
        for (kernelY = -radius; kernelY <= radius; kernelY++) {
            int imageX = x - kernelX;
            int imageY = y - kernelY;
            double exponentNumerator = -(kernelX * kernelX + kernelY * kernelY);
            double exponentDenominator = (2 * sigma * sigma);
            double eExpression = exp(exponentNumerator / exponentDenominator);
            double kernelValue = (eExpression / (2 * M_PI * sigma * sigma));
            RGBA pixel = image->pixels[imageY * width + imageX];

            redValue += pixel.r * kernelValue;
            greenValue += pixel.g * kernelValue;
            blueValue += pixel.b * kernelValue;
        }
    }
    *r = redValue/kernelSum;
    *g = greenValue/kernelSum;
    *b = blueValue/kernelSum;
}

void convolve(int width, int radius, int x, int y, Image* image, int kernelWidth, double *kernel, double *r, double *g, double *b) {
    int kernelX, kernelY;
    double redValue = 0.0;
    double greenValue = 0.0;
    double blueValue = 0.0;

    // #pragma omp parallel for collapse(2) private(kernelX, kernelY) schedule(static) reduction(+: redValue, greenValue, blueValue)
    for (kernelX = -radius; kernelX <= radius; kernelX++) {
        for (kernelY = -radius; kernelY <= radius; kernelY++) {
            int imageX = x - kernelX;
            int imageY = y - kernelY;
            double kernelValue = kernel[(kernelX + radius) * kernelWidth + (kernelY + radius)];
            RGBA pixel = image->pixels[imageY * width + imageX];

            redValue += pixel.r * kernelValue;
            greenValue += pixel.g * kernelValue;
            blueValue += pixel.b * kernelValue;
        }
    }
    *r = redValue;
    *g = greenValue;
    *b = blueValue;
}


Image *createBlurredImage_P(int radius, Image *image) {
    int width = image->width;
    int height = image->height;
    Image *outputImage = (Image *)malloc(sizeof(Image));
    outputImage->width = width;
    outputImage->height = height;
    outputImage->pixels = (RGBA *)malloc(width * height * sizeof(RGBA));

    double sigma = fmax(radius / 2.0, 1.0);
    int kernelWidth = (2 * radius) + 1;
    double *kernel;
    double sum;
    int x, y;
    // double redValue = 0.0;
    // double greenValue = 0.0;
    // double blueValue = 0.0;
    RGBA *outputPixel;
    double r, g, b;
    

    createGaussianKernel_P(radius, sigma, &kernel, &sum);

    #pragma omp parallel for collapse(2) private(x, y, outputPixel, r, g, b) firstprivate(radius, width) shared(kernel, image) schedule(static) //num_threads(8)
    for (x = radius; x < width - radius; x++) {
        for (y = radius; y < height - radius; y++) {
            // double r, g, b;
            convolve(width, radius, x, y, image, kernelWidth, kernel, &r, &g, &b);
            // convolve_new(width, radius, x, y, image, kernelWidth, &r, &g, &b, sigma, sum);

            outputPixel = &outputImage->pixels[y * width + x];
            outputPixel->r = (unsigned char)r;
            outputPixel->g = (unsigned char)g;
            outputPixel->b = (unsigned char)b;
            outputPixel->a = image->pixels[y * width + x].a; // Preserve the alpha channel
        }
    }

    free(kernel);
    return outputImage;
}

// Image *createBlurredImage_P(int radius, Image *image) {
//     int width = image->width;
//     int height = image->height;
//     Image *outputImage = (Image *)malloc(sizeof(Image));
//     outputImage->width = width;
//     outputImage->height = height;
//     outputImage->pixels = (RGBA *)malloc(width * height * sizeof(RGBA));

//     double sigma = fmax(radius / 2.0, 1.0);
//     int kernelWidth = (2 * radius) + 1;
//     double *kernel;
//     double sum;
//     int x, y, kernelX, kernelY;
//     double redValue = 0.0;
//     double greenValue = 0.0;
//     double blueValue = 0.0;
//     RGBA *outputPixel;
//     double r, g, b;
    

//     createGaussianKernel_P(radius, sigma, &kernel, &sum);

//     #pragma omp parallel for collapse(2) num_threads(20)
//     for (int iy = 0; iy < oh; ++iy) {
//         for (int ix = 0; ix < ow; ++ix) {
//             double red = 0.0;
//             double green = 0.0;
//             double blue = 0.0;
//             for (int c = 0; c < ic; ++c) {
//                 int f_raw_offset = c * filter.w * filter.h;
//                 int i_offset = c * iw * ih + iy * iw + ix;
//                 for (int fr = 0; fr < filter.h; ++fr) {
//                     for (int fc = 0; fc < filter.w; ++fc) {
//                         accum += kernel[f_raw_offset + fc] * image->pixels[i_offset + fc];
//                     }
//                     f_raw_offset += filter.w;
//                     i_offset += iw;
//                 }
//             }
//             // store output
//             outputs[b][oh * ow * f + iy * ow + ix] = accum;
//         }
//     }


//     free(kernel);
//     return outputImage;
// }
