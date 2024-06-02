// 316443902 Nir Koren
#include <immintrin.h>
#include <math.h>

float formula1(float *x, unsigned int length){
    __m128 root_sum = _mm_setzero_ps();
    __m128 product = _mm_set1_ps(1.0f);
    __m128 ones = _mm_set1_ps(1.0f);
    __m128 zeros = _mm_setzero_ps();

    for (long i = 0; i < length; i+=4) {
        __m128 x_p = _mm_load_ps(x + i);
        root_sum = _mm_add_ps(root_sum, _mm_sqrt_ps(x_p));
        product = _mm_mul_ps(product, _mm_add_ps(ones, _mm_mul_ps(x_p, x_p)));
    }

    root_sum = _mm_hadd_ps(root_sum, zeros);
    root_sum = _mm_hadd_ps(root_sum, zeros);

    product = _mm_mul_ps(product, _mm_shuffle_ps(product, product, 14));
    product = _mm_mul_ps(product, _mm_shuffle_ps(product, product, 1));
    
    float root_sum_float = _mm_cvtss_f32(root_sum);
    float product_float = _mm_cvtss_f32(product);

    return sqrtf(1 + cbrtf(root_sum_float) / product_float);
}