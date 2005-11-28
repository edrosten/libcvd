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
		template<class To, class From> struct int_info
		{
			//Difference in number of bits used
			static const int diff=traits<To>::bits_used - traits<From>::bits_used;
				
			//Which way do we need to shift
			static const int shift_dir =   (diff == 0)?0:((diff > 0)?1:-1);
		};
		
		template<class To, class From, int i> struct shift_convert
		{
			static To from(From f)
			{
				return static_cast<To>(f);
			}
		};
		
		template<class To, class From> struct shift_convert<To, From, 1>
		{
			static To from(From f)
			{
				return static_cast<To>(f) << int_info<To,From>::diff;
			}
		};

		template<class To, class From> struct shift_convert<To, From,-1>
		{
			static To from(From f)
			{
				return static_cast<To>(f >> -int_info<To,From>::diff);
			}
		};

		//Conversion for non integral types
		template<class To, class From, int i> struct maybe_integral_conv
		{
			static To from(From f)
			{
				return static_cast<To>(f*traits<To>::max_intensity/traits<From>::max_intensity); 
			}
		};
		
		//Convertion for integral types
		template<class To, class From> struct maybe_integral_conv<To, From, 1>
		{
			static To from(From f)
			{
					return shift_convert<To, From, int_info<To,From>::shift_dir>::from(f);
			}

		};
	}
	template<class To, class From> class scalar_convert
	{
		public:
		
		static To from(From f)
		{
			return Internal::maybe_integral_conv<To, From, is_int_to_int>::from(f);
		}

		//private:

		//True if converting from integral type to integral type
		static const bool is_int_to_int = traits<To>::integral && traits<From>::integral;
	};

	template<class C> class scalar_convert<C,C>
	{
		public:
		
		static C from(C f)
		{
			return f;
		}

		static const bool is_int_to_int = traits<C>::integral;
	};

}
}
#endif
