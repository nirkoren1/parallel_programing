// gcc -fopenmp guassonFilter.c guassonFilter.h stb_image_write.h stb_image.h main.c -lm -o sequential


#include "guassonFilter.h"
// #include "guassonFilterParallel.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


int main() {
    const char *inputFilename = "input_image.png";
    const char *outputFilename = "output_image1.png";
    int blurRadius = 5;
    clock_t start, end;
    double cpu_time_used1, cpu_time_used2;
    Image *outputImage;


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

    saveImage(outputFilename, outputImage);
    free(inputImage->pixels);
    free(inputImage);

    inputImage = loadImage(inputFilename);

    free(outputImage->pixels);
    free(outputImage);

    return 0;
}
