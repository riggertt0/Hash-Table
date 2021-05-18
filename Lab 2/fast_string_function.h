#pragma once
#include "header.h"


inline size_t fast_strlen(const char* str)
{
    unsigned int counter = 0;
    const __m256i* ptr = (__m256i*) str;
    __m256i zero = _mm256_setzero_si256();

    while (1)
    {
        __m256i mask = _mm256_cmpeq_epi8(*ptr, zero);

        if (!_mm256_testz_si256(mask, mask))
            return (counter * 32) + strlen((char*)ptr);

        counter++;
        ptr++;
    }
}

int fast_strcmp(const char* s1, const char* s2)
{
    if (s1 == s2)
        return 0;

    __m128i* ptr1 = reinterpret_cast<__m128i*>(const_cast<char*>(s1));
    __m128i* ptr2 = reinterpret_cast<__m128i*>(const_cast<char*>(s2));

    for (; ; ptr1++, ptr2++)
    {

        const __m128i a = _mm_loadu_si128(ptr1);
        const __m128i b = _mm_loadu_si128(ptr2);

        const uint8_t mode =
            _SIDD_UBYTE_OPS |
            _SIDD_CMP_EQUAL_EACH |
            _SIDD_NEGATIVE_POLARITY |
            _SIDD_LEAST_SIGNIFICANT;

        if (_mm_cmpistrc(a, b, mode))
        {
            const auto idx = _mm_cmpistri(a, b, mode);

            const uint8_t b1 = (reinterpret_cast<char*>(ptr1))[idx];
            const uint8_t b2 = (reinterpret_cast<char*>(ptr2))[idx];

            if (b1 < b2)
                return -1;
            else if (b1 > b2)
                return +1;
            else
                return 0;
        }
        else if (_mm_cmpistrz(a, b, mode))
            break;
    }

    return 0;
}
