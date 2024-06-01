#include <immintrin.h>
#include <stdint.h>

#define MAX_STR 256

static const __m256i mask_alpha = _mm256_set1_epi8(0x3f);
static const __m256i mask_num = _mm256_set1_epi8(0x0c);
static const __m256i mask_plus = _mm256_set1_epi8(0x3e);
static const __m256i mask_slash = _mm256_set1_epi8(0x3f);
static const __m256i add_alpha = _mm256_set1_epi8('A');
static const __m256i add_num = _mm256_set1_epi8('0' + 26 - 52);

int b64_distance(char str1[MAX_STR], char str2[MAX_STR]) {
    __m256i acc1 = _mm256_setzero_si256();
    __m256i acc2 = _mm256_setzero_si256();
    __m256i tmp;

    for (int i = 0; i < MAX_STR; i += 32) {
        __m256i in1 = _mm256_loadu_si256((__m256i *)(str1 + i));
        __m256i in2 = _mm256_loadu_si256((__m256i *)(str2 + i));

        tmp = _mm256_and_si256(_mm256_cmpgt_epi8(in1, mask_alpha), _mm256_cmplt_epi8(in1, mask_num));
        acc1 = _mm256_mullo_epi32(acc1, _mm256_set1_epi32(64));
        acc1 = _mm256_add_epi32(acc1, _mm256_sub_epi8(in1, add_num));
        acc1 = _mm256_and_si256(acc1, tmp);

        tmp = _mm256_or_si256(_mm256_cmpeq_epi8(in1, mask_plus), _mm256_cmpeq_epi8(in1, mask_slash));
        acc1 = _mm256_mullo_epi32(acc1, _mm256_set1_epi32(64));
        acc1 = _mm256_add_epi32(acc1, _mm256_sub_epi8(tmp, _mm256_set1_epi8(62)));

        tmp = _mm256_and_si256(_mm256_cmpgt_epi8(in2, mask_alpha), _mm256_cmplt_epi8(in2, mask_num));
        acc2 = _mm256_mullo_epi32(acc2, _mm256_set1_epi32(64));
        acc2 = _mm256_add_epi32(acc2, _mm256_sub_epi8(in2, add_num));
        acc2 = _mm256_and_si256(acc2, tmp);

        tmp = _mm256_or_si256(_mm256_cmpeq_epi8(in2, mask_plus), _mm256_cmpeq_epi8(in2, mask_slash));
        acc2 = _mm256_mullo_epi32(acc2, _mm256_set1_epi32(64));
        acc2 = _mm256_add_epi32(acc2, _mm256_sub_epi8(tmp, _mm256_set1_epi8(62)));
    }

    int32_t sum1 = _mm256_extract_epi32(acc1, 0) + _mm256_extract_epi32(acc1, 4) +
                   _mm256_extract_epi32(acc1, 2) + _mm256_extract_epi32(acc1, 6);
    int32_t sum2 = _mm256_extract_epi32(acc2, 0) + _mm256_extract_epi32(acc2, 4) +
                   _mm256_extract_epi32(acc2, 2) + _mm256_extract_epi32(acc2, 6);

    return sum2 - sum1;
}