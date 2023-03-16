//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  rgb.h                                                               //
//                                                                      //
//  Contains definitions of Rgb template class                          //
//                                                                      //
//  derived from IPRS_* developed by Tom Drummond                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef CVD_RGB_H
#define CVD_RGB_H

#include <iostream>

#include <cvd/byte.h>
#include <cvd/internal/concepts.h>

namespace CVD
{

/// A colour consisting of red, green and blue components.
/// Often used to store 24-bit colour information, i.e. <code>CVD::Rgb<CVD::byte></code>
/// @param T The datatype of each component
/// @ingroup gImage
template <Numeric T>
struct Rgb
{
	/// Constructs a colour as specified
	/// @param r The red component
	/// @param g The green component
	/// @param b The blue component
	inline Rgb(T r, T g, T b)
	    : red(r)
	    , green(g)
	    , blue(b)
	{
	}

	Rgb() = default;
	Rgb(const Rgb&) = default;
	Rgb& operator=(const Rgb&) = default;

	inline bool operator==(const Rgb<T>&) const = default;
	inline bool operator!=(const Rgb<T>&) const = default;

	template<Numeric To>
	operator Rgb<To>() const{
		return Rgb<To>(
			static_cast<To>(red),
			static_cast<To>(green),
			static_cast<To>(blue)
		);
	}

	T red; ///< The red component
	T green; ///< The green component
	T blue; ///< The blue component

};

template<Numeric Lhs, Numeric Rhs>
auto operator+(const Rgb<Lhs>& lhs, const Rgb<Rhs>& rhs)->Rgb<decltype(lhs.red+rhs.red)>{
	return {lhs.red+rhs.red, lhs.green+rhs.green, lhs.blue,rhs.blue};
}

template<Numeric Lhs, Numeric Rhs>
auto operator*(const Rgb<Lhs>& lhs, const Rhs& rhs)->Rgb<decltype(lhs.red+rhs)>{
	return {lhs.red*rhs, lhs.green*rhs, lhs.blue*rhs};
}




/// Write the colour to a stream in the format "(red,green,blue)"
/// @param os The stream
/// @param x The colour object
/// @relates Rgb
template <class T>
std::ostream& operator<<(std::ostream& os, const Rgb<T>& x)
{
	return os << "(" << x.red << "," << x.green << ","
	          << x.blue << ")";
}

/// Read a colour from a stream, interpreting three numbers as <code>byte</code>s
/// @param os The stream
/// @param x The colour object
/// @relates Rgb
inline std::ostream& operator<<(std::ostream& os, const Rgb<byte>& x)
{
	return os << "(" << static_cast<int>(x.red) << ","
	          << static_cast<int>(x.green) << ","
	          << static_cast<int>(x.blue) << ")";
}

} // end namespace
#endif
