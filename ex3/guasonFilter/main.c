#include "guassonFilter.h"
#include "guassonFilterParallel.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


int main() {
    const char *inputFilename = "input_image.png";
    const char *outputFilename1 = "output_image1.png";
    const char *outputFilename2 = "output_image2.png";
    int blurRadius = 5;
    clock_t start, end;
    double cpu_time_used1, cpu_time_used2;
    Image *outputImage;
    Image *outputImage_P;


    Image *inputImage = loadImage(inputFilename);
    if (inputImage == NULL) {
        printf("Failed to load image\n");
        return -1;
    }

    int count = 1;
    start = clock();
    for (int i = 0; i < count; i++)
    {
    outputImage = createBlurredImage(blurRadius, inputImage);
        
    }
    end = clock();
    cpu_time_used1 = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Avg time taken for serial part: %f seconds\n", cpu_time_used1/count);

    saveImage(outputFilename1, outputImage);

    inputImage = loadImage(inputFilename);

    start = clock();
    for (int i = 0; i < count; i++)
    {
    outputImage_P = createBlurredImage_P(blurRadius, inputImage);
    }
    end = clock();
    cpu_time_used2 = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Avg time taken for Parallel part: %f seconds\n", cpu_time_used2/count);

    saveImage(outputFilename2, outputImage_P);

    free(inputImage->pixels);
    free(inputImage);
    free(outputImage->pixels);
    free(outputImage);
    free(outputImage_P->pixels);
    free(outputImage_P);

    return 0;
}
