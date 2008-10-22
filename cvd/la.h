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
#ifndef CVD_LA_H
#define CVD_LA_H

#include <iostream>
#include <cvd/internal/is_pod.h>

namespace CVD {


  //////////////////////////////
  // CVD::La
  // Template class to represent luminance and alpha components
  //
  /// A colour consisting of luminance and alpha components
  /// @param T The datatype of each component
  /// @ingroup gImage
  template <typename T>
    class La
    {
    public:
      /// Default constructor. Does nothing.
      La() {}
      /// Constructs a colour as specified
      /// @param l The luminance component
      /// @param a The alpha component
    La(T l, T a) : luminance(l), alpha(a) {}

      T luminance; ///< The luminance component
      T alpha; ///< The alpha component

      /// Assignment operator
      /// @param c The colour to copy from
      La<T>& operator=(const La<T>& c)
	{luminance = c.luminance; alpha = c.alpha; return *this;}

      /// Assignment operator between two different storage types, using the standard casts as necessary
      /// @param c The colour to copy from
      template <typename T2>
	La<T>& operator=(const La<T2>& c){
	luminance = static_cast<T>(c.luminance);
	alpha = static_cast<T>(c.alpha); 
	return *this;
      }
   
      /// Logical equals operator. Returns true if each component is the same.
      /// @param c La to compare with
      bool operator==(const La<T>& c) const
      {return luminance == c.luminance && alpha == c.alpha;}
      
      /// Logical not-equals operator. Returns true unless each component is the same.
      /// @param c La to compare with
      bool operator!=(const La<T>& c) const
      {return luminance != c.luminance || alpha != c.alpha;}
      
    };
  
  /// Write the colour to a stream in the format "(luminance,alpha)"
  /// @param os The stream
  /// @param x The colour object
  /// @relates La
  template <typename T>
    std::ostream& operator <<(std::ostream& os, const La<T>& x)
    {
      return os << "(" << x.luminance << "," << x.alpha << ")";
    }
  
  /// Write the colour to a stream in the format "(luminance,alpha)"
  /// @param os The stream
  /// @param x The colour object
  /// @relates La
  inline std::ostream& operator <<(std::ostream& os, const La<unsigned char>& x)
  {
    return os << "(" << static_cast<unsigned int>(x.luminance) << ","
	      << static_cast<unsigned int>(x.alpha) << ")";
  }
  
#ifndef DOXYGEN_IGNORE_INTERNAL
  namespace Internal
  {
    template<class C> struct is_POD<La<C> >
      {
	enum { is_pod = is_POD<C>::is_pod };
      };
  }
#endif
  
  
  
} // end namespace 
#endif

