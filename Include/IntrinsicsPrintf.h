#ifndef INTRINSICS_PRINTF_H
#define INTRINSICS_PRINTF_H

#include "./Config.h"
#include <immintrin.h>

inline void printf_m128( __m128 a );
inline void printf_m128i(__m128i a);

inline void printf_m256( __m256 a );
inline void printf_m256i(__m256i a);

inline void printf_m512( __m512 a );
inline void printf_m512i(__m512i a);

#endif
