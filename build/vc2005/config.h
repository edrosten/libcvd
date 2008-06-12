#ifndef CVD_CONFIG_H
#define CVD_CONFIG_H

#define CVD_ARCH_LITTLE_ENDIAN 1
#if 0  // not a 64 bit system, potentially enable otherwise
#ifndef CVD_DISABLE_CPU_x86_64
    #define CVD_HAVE_CPU_x86_64 1
#endif
#endif
#if 0 // for now would like to find a solution
#ifndef CVD_DISABLE_GLOB
    #define CVD_HAVE_GLOB 1
#endif
#endif

#ifndef CVD_DISABLE_JPEG
    #define CVD_HAVE_JPEG 1
#endif
#ifndef CVD_DISABLE_LAPACK
    #define CVD_HAVE_LAPACK 1
#endif
#ifndef CVD_DISABLE_MMX
    #define CVD_HAVE_MMX 1
#endif
#ifndef CVD_DISABLE_MMXEXT
    #define CVD_HAVE_MMXEXT 1
#endif

#if 0  // FIXME: compile libz and libpng
#ifndef CVD_DISABLE_PNG
    #define CVD_HAVE_PNG 1
#endif
#endif

#ifndef CVD_DISABLE_PTHREAD
    #define CVD_HAVE_PTHREAD 1
#endif

#if 0   // look into that pthred-win32 doesn't have that
#ifndef CVD_DISABLE_PTHREAD_YIELD
    #define CVD_HAVE_PTHREAD_YIELD 1
#endif
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

#if 0   // no TIFF for now 
#ifndef CVD_DISABLE_TIFF
    #define CVD_HAVE_TIFF 1
#endif
#endif

#ifndef CVD_DISABLE_TOON
    #define CVD_HAVE_TOON 1
#endif
#ifndef CVD_DISABLE_HALFSAMPLE_SSE16
    #define CVD_INTERNAL_HAVE_HALFSAMPLE_SSE16 1
#endif
#ifndef CVD_DISABLE_TIFF_ORIENTED
    #define CVD_INTERNAL_HAVE_TIFF_ORIENTED 1
#endif
#ifndef CVD_DISABLE_YV402P_MMX
    #define CVD_INTERNAL_HAVE_YV402P_MMX 1
#endif
#define CVD_INTERNAL_JPEG_BUFFER_SIZE 1

/*
#define CVD_KERNEL_MAJOR 2
#define CVD_KERNEL_MINOR 6
#define CVD_KERNEL_RELEASE 22
#define CVD_MAJOR_VERSION 0
#define CVD_MINOR_VERSION 7
*/

// make sure to have all necessary definitions included, this requires min Windows 2000
#define _WIN32_WINNT 0x0500

// missing C99 define (C99 not supported in VC++)
typedef __int32 int32_t;

// avoid warnings about using old unsave versions of libc functions
#define _CRT_SECURE_NO_WARNINGS

// avoid warnings about checked/unchecked iterators
#define _SCL_SECURE_NO_WARNINGS

// to get M_PI, etc.
#define _USE_MATH_DEFINES   

// don't include garbage
#define WINDOWS_LEAN_AND_MEAN

// don't define macros for min, max
#define NOMINMAX

#endif //  CVD_CONFIG_WIN_H
