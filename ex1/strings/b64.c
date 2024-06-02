// 316443902 Nir Koren
#include <emmintrin.h> 
#include <nmmintrin.h> 
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#define MAX_STR 256


__m128i mult(__m128i a, __m128i b) {
    __m128i a_lo = _mm_unpacklo_epi8(a, _mm_setzero_si128());
    __m128i a_hi = _mm_unpackhi_epi8(a, _mm_setzero_si128());
    __m128i b_lo = _mm_unpacklo_epi8(b, _mm_setzero_si128());
    __m128i b_hi = _mm_unpackhi_epi8(b, _mm_setzero_si128());

    // Perform multiplication
    __m128i mul_lo = _mm_mullo_epi16(a_lo, b_lo);
    __m128i mul_hi = _mm_mullo_epi16(a_hi, b_hi);

    // Optional: Pack back to 8-bit, selecting parts of the product (e.g., low bytes)
    __m128i result = _mm_packus_epi16(mul_lo, mul_hi);
    return result;
}

// Function to filter and convert a base64 string to its base10 number
uint64_t b64_to_base10(const char* str, int len) {
    __m128i vecA_R = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Z', 'A');
    __m128i veca_R = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'z', 'a');
    __m128i vec0_R = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '9', '0');
    __m128i vecP_R = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '+', '+');
    __m128i vecS_R = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/', '/');
    
    __m128i vec26 = _mm_set1_epi8(26);
    __m128i vec52 = _mm_set1_epi8(52);
    __m128i vec62 = _mm_set1_epi8(62);
    __m128i vec63 = _mm_set1_epi8(63);

    __m128i vecA = _mm_set1_epi8('A');
    __m128i veca = _mm_set1_epi8('a');
    __m128i vec0 = _mm_set1_epi8('0');
    __m128i vecp = _mm_set1_epi8('p');
    __m128i vecs = _mm_set1_epi8('s');

    uint64_t result = 0;

    for (int i = 0; i < len; i += 16) {
        __m128i data = _mm_loadu_si128((__m128i*)(str + i));

        __m128i upper = _mm_cmpistrm(vecA_R, data, 0b01000100);
        __m128i lower = _mm_cmpistrm(veca_R, data, 0b01000100);
        __m128i numbers = _mm_cmpistrm(vec0_R, data, 0b01000100);
        __m128i plusses = _mm_cmpistrm(vecP_R, data, 0b01000100);
        __m128i slashes = _mm_cmpistrm(vecS_R, data, 0b01000100);

        __m128i mask = _mm_set1_epi8(1);
        upper = _mm_and_si128(upper, mask);
        lower = _mm_and_si128(lower, mask);
        numbers = _mm_and_si128(numbers, mask);
        plusses = _mm_and_si128(plusses, mask);
        slashes = _mm_and_si128(slashes, mask);

        upper = _mm_sub_epi8(mult(upper, data), mult(upper, vecA));
        lower = _mm_add_epi8(_mm_sub_epi8(mult(lower, data), mult(lower, veca)), mult(lower, vec26));
        numbers = _mm_add_epi8(_mm_sub_epi8(mult(numbers, data), mult(numbers, vec0)), mult(numbers, vec52));
        plusses = mult(plusses, vec62);
        slashes = mult(slashes, vec63);


        __m128i all = _mm_set1_epi8(0);
        all = _mm_add_epi8(all, upper);
        all = _mm_add_epi8(all, lower);
        all = _mm_add_epi8(all, numbers);
        all = _mm_add_epi8(all, plusses);
        all = _mm_add_epi8(all, slashes);



        // Perform character filtering and converting here, adjusted for SSE3
        uint8_t* bytes = (uint8_t*)&all;
        for (int j = 0; j < 16; ++j) {
            uint8_t value = bytes[j];
            if (value > 0)
                result = result * 64 + value;
        }
    }
    return result;
}

// Compute base64 distance between two strings
int b64_distance(char str1[MAX_STR], char str2[MAX_STR]) {
    // int len1 = strlen(str1);
    // int len2 = strlen(str2);
    uint64_t num1 = b64_to_base10(str1, MAX_STR);
    uint64_t num2 = b64_to_base10(str2, MAX_STR);

    return (int)(num1 - num2);
}
