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
