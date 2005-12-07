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
#ifndef CVD_SCALAR_CONVERT_H
#define CVD_SCALAR_CONVERT_H

#include <cvd/byte.h>
#include <cvd/internal/pixel_traits.h>

namespace CVD
{
namespace Pixel
{

	namespace Internal
	{
		template<class To, class From> struct int_info {
		  //Difference in number of bits used
		  static const int diff=traits<To>::bits_used - traits<From>::bits_used;
		  
		  //Which way do we need to shift
		  static const int shift_dir =   (diff == 0)?0:((diff > 0)?1:-1);
		};
		
		template<class To, class From, int i=int_info<To,From>::shift_dir> struct shift_convert {
		  template <class D> static To from(D f) {
		    return static_cast<To>(f);
		  }		  
		};

		template<class To, class From> struct shift_convert<To, From, 1> {
		  template <class D> static To from(D f) {
		    return static_cast<To>(f) << int_info<To,From>::diff;
		  }
		};

		template<class To, class From> struct shift_convert<To, From,-1> {	
		  template <class D> static To from(D f)  {
		    return static_cast<To>(f >> -int_info<To,From>::diff);
		  }
		};
		
		
		static float float_for_byte[511];
		static double double_for_byte[511];
		
		template <class S> bool buildLookupTable(S table[]) {
		  for (int i=0; i<=511; i++)
		    table[i] = (i-255)/255.0;    
		  return true;
		}
		static bool init_float_for_byte = buildLookupTable(float_for_byte);
		static bool init_double_for_byte = buildLookupTable(double_for_byte);
		inline float byte_to_float(int b) { return float_for_byte[b+255]; }
		inline double byte_to_double(int b) { return double_for_byte[b+255]; }
		
		template <class From, class To, class D=From, bool int_to_int = traits<To>::integral && traits<From>::integral && traits<D>::integral> struct ScalarConvert {
		  static inline To from(const D& from) {
		    return static_cast<To>(from*traits<To>::max_intensity/traits<From>::max_intensity);
		  }
		};
		
		template <class From, class To, class D> struct ScalarConvert<From,To,D,true> {
		  static inline To from(const D& f) {
		    return shift_convert<To, From, int_info<To,From>::shift_dir>::from(f);
		  }
		};
		
		template <class D> struct ScalarConvert<byte,float,D,false> {
		  static inline float from(const D& from) {
		    return byte_to_float(from);
		  }
		};
		
		template <class D> struct ScalarConvert<byte,double,D,false> {
		  static inline double from(const D& from) {
		    return byte_to_double(from);
		  }
		};
		

	}
	

	template <class To, class From, class D> inline To scalar_convert(const D& d) { return Internal::ScalarConvert<From,To,D>::from(d); }
	//template <class To, class From> inline To scalar_convert(const From& d) { return Internal::ScalarConvert<From,To>::from(d); }
	
}
}
#endif
