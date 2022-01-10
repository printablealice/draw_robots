#ifndef POE_CPP
#define POE_CPP

////////////////////////////////////////////////////////////////////////////////
// just a messy file of helper macros and functions (will clean up by end)    //
////////////////////////////////////////////////////////////////////////////////

#define NITEMS(x) (sizeof(x) / sizeof(x[0]))   // number of items in fixed size array
#define MODULO(x, N) (((x) % (N) + (N)) % (N)) // modulo that works on negative numbers

#ifdef ASSERT_WAITS_FOR_YOU_TO_PRESS_ENTER_BEFORE_CRASHING
#define ASSERT(b) if (!(b)) { \
    printf("ASSERT Line %d in %s \n", __LINE__, __FILE__); \
    printf("press Enter to crash");                        \
    getchar();                                             \
    *((volatile int *) 0) = 0;                             \
}
#else
#define ASSERT(b) if (!(b)) { \
    printf("ASSERT Line %d in %s \n", __LINE__, __FILE__); \
    *((volatile int *) 0) = 0;                             \
}
#endif
#define STATIC_ASSERT(b) static_assert(b, "STATIC_ASSERT")

#define SGN(a) ((a) > 0 ? 1 : -1)
#define AVG(a, b) (.5 * (a) + .5 * (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define LERP(t, a, b) ((1 - (t)) * (a) + (t) * (b))
#define CLAMP(t, a, b) MIN(MAX(t, a), b)
#define MAG_CLAMP(t, a) CLAMP(t, -(a), a)
// #define CLAMPED_LERP(t, a, b) LERP(CLAMP(t, 0, 1), a, b)
#define INVERSE_LERP(p, a, b) (((p) - (a)) / ((b) - (a)))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MID(a, b, c) MAX(MIN((a), (b)), MIN(MAX((a), (b)), (c)))

#define RAD(a) ((a) * PI / 180)
#define DEG(a) ((a) * 180 / PI)

#define elif else if
#define for_(i, N) for (int i = 0; i < (N); ++i)
#define for____(N) for (int _ = 0; _ < (N); ++_)
#define stb_for_polygon(i, j, N_ij) for (int i = N_ij - 1, j = 0; j < N_ij; i = j, ++j)

#define SWAP(a, b) do {                   \
    ASSERT(sizeof(a) == sizeof(b));       \
    void *__SWAP_tmp = malloc(sizeof(a)); \
    memcpy(__SWAP_tmp, &a, sizeof(a));    \
    memcpy(&a, &b, sizeof(b));            \
    memcpy(&b, __SWAP_tmp, sizeof(b));    \
} while (0)

#define NOR(a, b) (!(a) && !(b))
#define XOR(a, b) (((a) && !(b)) || (!(a) && (b)))

#define TINY 1e-7
#define BIG 1e5
#define PI 3.14159265358979323846264338f
#define TWO_PI  6.28318530718f
#define IS_POSITIVE(a) ((a) > TINY)
#define IS_NEGATIVE(a) ((a) < -TINY)
#define IS_ZERO(a) (!IS_NEGATIVE(a) && !IS_POSITIVE(a))
// todo NON_NEGATIVE? NON_POSITIVE?
#define NON_ZERO(a) (!IS_ZERO(a))
#define IS_EQUAL(a, b) (ABS((a) - (b)) < TINY)
#define IN_RANGE(c, a, b) (((a) - TINY < (c)) && ((c) < (b) + TINY))
#define NOT_EQUAL(a, b) (!IS_EQUAL((a), (b)))
#define IS_EVEN(a) ((a) % 2 == 0)
#define IS_ODD(a) (!IS_EVEN(a))
#define IS_DIVISIBLE_BY(a, b) (int(a) % int(b) == 0)
#define NOT_DIVISIBLE_BY(a, b) (!IS_DIVISIBLE_BY((a), (b)))
#define IDBBNETZ(a, b) (((a) == 0) ? false : IS_DIVISIBLE_BY((a), (b))) // isDivisibleByButNotEqualToZero

#define MIAO_COS(f, F, a, b) LERP(.5 - .5 * cos(NUM_DEN(f, F) * TWO_PI), a, b)
#define MIAO_SIN(f, F, a, b) LERP(.5 + .5 * sin(NUM_DEN(f, F) * TWO_PI), a, b)

#define _CRT_SECURE_NO_WARNINGS
#define _HAS_EXCEPTIONS 0

typedef double real;
typedef unsigned char byte;
#include <stdint.h>
#define u64 uint64_t

#define RAND(a, b) LERP(real(rand()) / RAND_MAX, a, b)

#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#define NUM_DEN(f, F) (real(f) / (F))
#define NUM_DENm1(f, F) (real(f) / ((F) - 1))
#define NUMp1_DEN(f, F) (real((f) + 1) / (F))



/* hack to inspect array in debugger:
 points,*((int*)(points)-8)
 */
#pragma warning ( push )
#pragma warning ( disable : 4456 )
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#pragma warning ( pop ) 
#undef arrput
#undef arrpush
#undef arrlen
#undef arrlenu
#define _ARRPUT_(T) void arrput(T *&array, T element) { stbds_arrput(array, element); } // hack to allow curly braces in arguments, e.g. arrput(simplices, { 0, 1, 2 })
_ARRPUT_(int); // fornow
_ARRPUT_(real); // fornow
template <typename T> void arrput(T *&array, T element) { stbds_arrput(array, element); }

#define arrlen(array) int(stbds_arrlen(array))

template <typename T> T *poe_arr2raw(T *dyn) {
    int size = arrlen(dyn) * sizeof(T);
    T *raw = (T *) malloc(size);
    memcpy(raw, dyn, size);
    arrfree(dyn);
    return raw;
}



struct qsortHelperStruct {
    int index;
    real value;
};
void poe_sort_against(void *base, int nitems, int size, real *corresp_values_to_sort_against) {
    qsortHelperStruct *helperArray = (qsortHelperStruct *) calloc(sizeof(qsortHelperStruct), nitems); {
        for_(i, nitems) helperArray[i] = { i, corresp_values_to_sort_against[i] };
    }

    int(* comp)(qsortHelperStruct *, qsortHelperStruct *) \
        = [](qsortHelperStruct *a, qsortHelperStruct *b) -> int { return (a->value < b->value) ? -1 : 1; }; 

    qsort(helperArray, nitems, sizeof(qsortHelperStruct), (int (*)(const void *, const void *))comp);

    void *tmp_buffer = malloc(nitems * size); { // fornow
        for_(i, nitems) memcpy(\
                ((byte *) tmp_buffer) + (i * size), \
                ((byte *) base) + (helperArray[i].index * size), \
                size);
        memcpy(base, tmp_buffer, nitems * size);
    } free(tmp_buffer);
}


#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)
#define UNIQUE_VARIABLE_NAME CONCAT(_VAR_, __COUNTER__)
#define VAR_LINE CONCAT(VAR_, __LINE__)

#define BEGIN_PREMAIN_BLOCK static int UNIQUE_VARIABLE_NAME = []() {
#define END_PREMAIN_BLOCK return 1; }();

bool notSetTrueButReturnOriginalValue(bool &b) { bool ret = !b; b = true; return ret; }
#define EXECUTE_THIS_BLOCK_OF_CODE_NO_MORE_THAN_ONCE static bool VAR_LINE = false; if (notSetTrueButReturnOriginalValue(VAR_LINE))

#define for_inclusiveRange(i, a, b) for (int i = int(a); i <= int(b); ++i)
#define for_inclusiveRange_mp(i, a) for_inclusiveRange(i, -(a), a)
#define for_sign(i) for (int i = -1; i <= 1; i += 2)

#define Kilobytes(Value) (          Value *1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#include <ctime>
#define secondsSince1970() int(time(0))

#define EXPAND(x) x
#define PP_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define NARGS(...) EXPAND(PP_ARG_N(__VA_ARGS__,9,8,7,6,5,4,3,2,1,0))

struct int2 {
    int i, j;
    int &operator[](int index) {
        ASSERT(index == 0 || index == 1);
        if (index == 0) return i;
        return j;
    }
};
struct int3 {
    int i, j, k;
    int &operator[](int index) {
        ASSERT(index == 0 || index == 1 || index == 2);
        return (index == 0) ? i : (index == 1) ? j : k;
    }
};

#endif
