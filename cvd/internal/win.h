#ifndef CVD_INC_INTERNAL_WIN_H
#define CVD_INC_INTERNAL_WIN_H

	// make sure to have all necessary definitions included, this requires min Windows 2000
	#define _WIN32_WINNT 0x0500

	// missing C99 define (C99 not supported in VC++)
	typedef __int32 int32_t;

	// avoid warnings about using old unsave versions of libc functions
	#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#endif

	// avoid warnings about checked/unchecked iterators
	#ifndef _SCL_SECURE_NO_WARNINGS
	#define _SCL_SECURE_NO_WARNINGS
	#endif

	// to get M_PI, etc.
	#define _USE_MATH_DEFINES

	// don't include garbage
	#define WINDOWS_LEAN_AND_MEAN

	// don't define macros for min, max
	#define NOMINMAX
#endif 
