/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef __RGB8_H
#define __RGB8_H

#include <cvd/internal/is_pod.h>


namespace CVD {

/// @ingroup gImage

/// A 32-bit colour.
/// @b Deprecated The templated Rgba should be used instead (Rgba<byte> is exactly equivalent).
struct Rgb8
{
   unsigned char red; ///< The red component
   unsigned char green; ///< The green component
   unsigned char blue; ///< The blue component
   unsigned char dummy; ///< The 4th byte, usually either ignored or used to represent the alpha value

   /// Default constructor. Sets all elements to zero.
   explicit Rgb8() : red(0), green(0), blue(0), dummy(0) {}

   /// Construct an Rgb8 as specified
   /// @param r The red component
   /// @param g The green component
   /// @param b The blue component
   /// @param a The dummy byte (defaults to zero)
   explicit Rgb8(unsigned char r, unsigned char g, unsigned char b, unsigned char a=0)
   : red(r), green(g), blue(b), dummy(a) {}
};

#ifndef DOXYGEN_IGNORE_INTERNAL
namespace Internal
{
  template <> struct is_POD<Rgb8>
  {
    enum { is_pod = 1 };
  };
}
#endif

} // end namespace 
#endif
