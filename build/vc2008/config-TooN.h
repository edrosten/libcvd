#ifndef CVD_CONFIG_H
#define CVD_CONFIG_H
#ifdef WIN32
	#include <cvd/internal/win.h>
#endif
#define CVD_ARCH_LITTLE_ENDIAN 1
#ifndef CVD_DISABLE_CPU_x86_64
    #define CVD_HAVE_CPU_x86_64 1
#endif
#ifndef CVD_DISABLE_FENV_H
    #define CVD_HAVE_FENV_H 1
#endif
#ifndef CVD_DISABLE_GLOB
    #define CVD_HAVE_GLOB 1
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
#ifndef CVD_DISABLE_POSIX_RT
    #define CVD_HAVE_POSIX_RT 1
#endif
#ifndef CVD_DISABLE_PTHREAD
    #define CVD_HAVE_PTHREAD 1
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
#ifndef CVD_DISABLE_VIDEODISPLAY
    #define CVD_HAVE_VIDEODISPLAY 1
#endif
#define CVD_INTERNAL_NEED_TR1 1
#define CVD_MAJOR_VERSION 0
#define CVD_MINOR_VERSION 8
#endif
