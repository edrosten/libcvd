/*******************************************************************************

	transcribe_luminance_types.h

This file contains inline template functions to convert from one scalar image
type to another. This can also be used to convert scalar elements of composite
(eg RGB) images

The functions need to be able to convert from uchar and ushort to any type and
from any type to ushort and uchar, sice ushort and uchar are the basic PNM 
types. For pedants, uchar is one octet and ushort is 2 octets.

The conversion works like this:

input types (uc and us) are 0--255 and 0--65563

output types are 0--1 for float types and 0--MAX for int types. For signed 
types the bottom half is unused by this.

Upconversion of integer types is done with a bitshift, so if the input has 8 
bits and the output has 16 bits, the maximum value that will be outputted is
0xff00. This means that true black is preserved and true white is not.

*******************************************************************************/
#ifndef PNM_LOAD_BUILTIN_H
#define PNM_LOAD_BUILTIN_H

#include <limits>

namespace CVD
{
namespace PNM
{


//int and short myst be 1 and 2 octets in size.
template<class T> struct my_limits: public std::numeric_limits<T>
{
};

template<> struct my_limits<unsigned char>: public std::numeric_limits<unsigned char>
{
	static const int digits=8;
	static unsigned char max() throw(){return 0xff;}
};

template<> struct my_limits<unsigned short>: public std::numeric_limits<unsigned short>
{
	static const int digits=16;
	static unsigned short max() throw(){return 0xffff;}
};

////////////////////////////////////////////////////////////////////////////////
//
//Convert integer types
//

inline int fast_abs(int i)
{
	//This gets optimized away in a decent compiler
	if(i<0)i=-i;
	return i;
}

template<class C, class D> inline C pix_convert_lum(D pxl)
{
	unsigned const int c = my_limits<C>::digits;
	unsigned const int d = my_limits<D>::digits;

	//We need tha abs here otherwise g++ (3.1.1) complains with a rather odd
	//error message: it complains that the shift is greater than the width of
	//the type. negative shifts are otherwise implmentation dependent.

	if(c > d)
		return (C)pxl << fast_abs(c-d);
	else
		return pxl >> fast_abs(d-c);
}

////////////////////////////////////////////////////////////////////////////////
//
// This one should never be called, but just in case...
//

//template<class C> inline C pix_convert_lum(C pxl)
//{
//	return pxl;
//}


////////////////////////////////////////////////////////////////////////////////
//
// Float to float conversions

#define PNM_FLOAT_TO_FLOAT(X,Y)\
template<> inline X pix_convert_lum<X,Y>(Y pxl){return (X)pxl;}\
template<> inline Y pix_convert_lum<Y,X>(X pxl){return (Y)pxl;}

PNM_FLOAT_TO_FLOAT(float, double)
PNM_FLOAT_TO_FLOAT(float, long double)
PNM_FLOAT_TO_FLOAT(double, long double)


////////////////////////////////////////////////////////////////////////////////
//
//Convert integer types to and from floating point types
//

/*#define PNM_INT_FLOAT(F)													\
template<class I> inline F pix_convert_lum<F,I>(I);							\
template<class I> inline F pix_convert_lum<F,I>(I pxl)						\
{																			\
	static const F mul=1/(F)my_limits<I>::max();							\
	return pxl * mul;														\
}																			\
template<class I> inline I pix_convert_lum<I,F>(F pxl);						\
template<class I> inline I pix_convert_lum<I,F>(F pxl)						\
{																			\
	static const F mul=(F)my_limits<I>::max();								\
	return static_cast<I>(pxl * mul);										\
}																			\

PNM_INT_FLOAT(float)
PNM_INT_FLOAT(double)
PNM_INT_FLOAT(long double)
*/


#define PNM_FLOAT_CONV(F,T,M) \
template<> inline F pix_convert_lum(unsigned T pixel)\
{\
	const F mul = 1/(F)M;\
	return pixel * mul;\
}\
\
template<> inline unsigned T pix_convert_lum(F pixel)\
{\
	return static_cast<unsigned T>(pixel * M);\
}

PNM_FLOAT_CONV(float, 		char, 	0xff)
PNM_FLOAT_CONV(double, 		char, 	0xff)
PNM_FLOAT_CONV(long double,	char, 	0xff)
PNM_FLOAT_CONV(float, 		short, 	0xffff)
PNM_FLOAT_CONV(double, 		short, 	0xffff)
PNM_FLOAT_CONV(long double,	short, 	0xffff)

#undef PNM_FLOAT_CONV

}
}
#endif


