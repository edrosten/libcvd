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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef CVD_BUILTIN_TRAITS_H_
#define CVD_BUILTIN_TRAITS_H_

#include <limits>

namespace CVD
{
	namespace Pixel
	{

		//The "Component" class gives us information about the pixel components, 
		//ie how many components there are and whay the type is.
		
		//We use a 2 layer thing here so that Component is only properly defined
		//for builtin types, unless explicitly overridden.

		template<class P, int spp> struct component_base
		{
			static const int count=component_information_is_unknown_for_this_class;
			typedef  typename P::component_type_is_unknown_for_this_class type;
		};

		template<class P> struct component_base<P, 1>
		{
			typedef P type;
			static const unsigned int count = 1;

			static const P& get(const P& pixel, int i)
			{
				return pixel;
			}

			static P& get(P& pixel, int i)
			{
				return pixel;
			}
			
		};

		template<class P> struct Component: public component_base<P, std::numeric_limits<P>::is_specialized>
		{
		};
		
	}
}

#endif
