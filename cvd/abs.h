#ifndef CVD_ABS_H
#define CVD_ABS_H

namespace CVD
{
	/// Local version of abs. Returns the absolute value of a variable.
	/// We provide our own version so that we can produce instantiations
	/// that correctly work for unsigned datatypes
	/// @param T The type of the variable
	/// @param t The input parameter
	/// @ingroup gCPP
	template <class T> 
	inline T abs(T t) { return t<0 ? -t : t; }
	/// Instantiation of abs for unsigned char, which returns an unsigned.
	/// @param b The input parameter
	/// @ingroup gCPP
	inline unsigned char abs(unsigned char b) { return b; }
	/// Instantiation of abs for unsigned short, which returns an unsigned.
	/// @param u The input parameter
	/// @ingroup gCPP
	inline unsigned short abs(unsigned short u) { return u; }
	/// Instantiation of abs for unsigned int, which returns an unsigned.
	/// @param u The input parameter
	/// @ingroup gCPP
	inline unsigned int abs(unsigned int u) { return u; }
	/// Instantiation of abs for unsigned long, which returns an unsigned.
	/// @param u The input parameter
	/// @ingroup gCPP
	inline unsigned long abs(unsigned long u) { return u; }
}
#endif

