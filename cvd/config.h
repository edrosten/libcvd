#ifndef CVD_CONFIG_H
#define CVD_CONFIG_H
#define CVD_ARCH_LITTLE_ENDIAN 1
#ifndef CVD_DISABLE_CPU_i386
    #define CVD_HAVE_CPU_i386 1
#endif
#ifndef CVD_DISABLE_GLOB
    #define CVD_HAVE_GLOB 1
#endif
#ifndef CVD_DISABLE_INLINE_ASM
    #define CVD_HAVE_INLINE_ASM 1
#endif
#ifndef CVD_DISABLE_LAPACK
    #define CVD_HAVE_LAPACK 1
#endif
#ifndef CVD_DISABLE_MEMALIGN
    #define CVD_HAVE_MEMALIGN 1
#endif
#ifndef CVD_DISABLE_MMX
    #define CVD_HAVE_MMX 1
#endif
#ifndef CVD_DISABLE_MMXEXT
    #define CVD_HAVE_MMXEXT 1
#endif
#ifndef CVD_DISABLE_PNG
    #define CVD_HAVE_PNG 1
#endif
#ifndef CVD_DISABLE_PTHREAD
    #define CVD_HAVE_PTHREAD 1
#endif
#ifndef CVD_DISABLE_PTHREAD_YIELD_NP
    #define CVD_HAVE_PTHREAD_YIELD_NP 1
#endif
#ifndef CVD_DISABLE_SSE
    #define CVD_HAVE_SSE 1
#endif
#ifndef CVD_DISABLE_SSE2
    #define CVD_HAVE_SSE2 1
#endif
#ifndef CVD_DISABLE_SSE3
    #define CVD_HAVE_SSE3 1
#endif
#ifndef CVD_DISABLE_TOON
    #define CVD_HAVE_TOON 1
#endif
#ifndef CVD_DISABLE_TR1_SHARED_PTR
    #define CVD_HAVE_TR1_SHARED_PTR 1
#endif
#ifndef CVD_DISABLE_VIDEODISPLAY
    #define CVD_HAVE_VIDEODISPLAY 1
#endif
#ifndef CVD_DISABLE_HALFSAMPLE_SSE16
    #define CVD_INTERNAL_HAVE_HALFSAMPLE_SSE16 1
#endif
#ifndef CVD_DISABLE_YV402P_MMX
    #define CVD_INTERNAL_HAVE_YV402P_MMX 1
#endif
#define CVD_MAJOR_VERSION 0
#define CVD_MINOR_VERSION 8
#endif
