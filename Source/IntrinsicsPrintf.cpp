#ifndef INTRINSICS_PRINTF_CPP
#define INTRINSICS_PRINTF_CPP

#include "../Include/Config.h"
#include <stdio.h>
#include "../Include/IntrinsicsPrintf.h"

inline void printf_m128(__m128 a)
{
    for (int i = 0; i < 4; i++) printf("%f ", ((float*)(&a))[i]);
    printf("\n");
}

inline void printf_m128i(__m128i a)
{
    for (int i = 0; i < 4; i++) printf("%d ", (Uint8)(((int*)(&a))[i]));
    printf("\n");
}

inline void printf_m256(__m256 a)
{
    for (int i = 0; i < 8; i++) printf("%f ", ((float*)(&a))[i]);
    printf("\n");
}

inline void printf_m256i(__m256i a)
{
    for (int i = 0; i < 8; i++) printf("%d ", (Uint8)(((int*)(&a))[i]));
    printf("\n");
}

inline void printf_m512(__m512 a)
{
    for (int i = 0; i < 16; i++) printf("%f ", ((float*)(&a))[i]);
    printf("\n");
}

inline void printf_m512i(__m512i a)
{
    for (int i = 0; i < 16; i++) printf("%d ", (Uint8)(((int*)(&a))[i]));
    printf("\n");
}

#endif
