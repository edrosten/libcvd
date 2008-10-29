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
#ifndef CVD_BUILTIN_TRAITS_H_
#define CVD_BUILTIN_TRAITS_H_

#include <limits>
#include <cstddef>

#if defined (CVD_HAVE_TOON)
#include <TooN/TooN.h>
#endif




namespace CVD
{
  namespace Pixel
  {

    //The "Component" class gives us information about the pixel components, 
    //ie how many components there are and whay the type is.
		
    //We use a 2 layer thing here so that Component is only properly defined
    //for builtin types, unless explicitly overridden.

    template<class P, int spp> struct component_base {
      template<bool x> struct component_base_only_for_basic_types;
      static const int fail = sizeof(component_base_only_for_basic_types<false>);
    };
	  
    template<class P> struct component_base<P, 1>
    {
    };

    //template <class P, int primitive = std::numeric_limits<P>::is_specialized> struct Component;

    template<class P> struct Component
    {
      typedef P type;
      static const size_t count = 1;
		  
      static const P& get(const P& pixel, size_t)
      {
	return pixel;
      }

      static P& get(P& pixel, size_t)
      {
	return pixel;
      }
			
    };

    template<class P, int I> struct Component<P[I]>
    {
      typedef P type;
      static const size_t count=I;
      
      static const P& get(const P pixel[I], size_t i)
      {
	return pixel[i];
      }

      static inline P& get(P pixel[I], size_t i)
      {
	return pixel[i];
      }
    };

#if defined (CVD_HAVE_TOON)
    template<int N> struct Component<TooN::Vector<N> >
    {
      typedef double type;
      static const size_t count=N;
      
      static const type& get(const TooN::Vector<N>& pixel, size_t i)
      {
	return pixel[i];
      }

      static inline type& get(TooN::Vector<N>& pixel, size_t i)
      {
	return pixel[i];
      }
    };

    template<int N, int M> struct Component<TooN::Matrix<N,M> >
    {
      typedef double type;
      static const size_t count=N*M;
      
      static const type& get(const TooN::Matrix<N,M>& pixel, size_t i)
      {
	return pixel[i/M][i%M];
      }

      static inline type& get(TooN::Matrix<N,M>& pixel, size_t i)
      {
	return pixel[i/M][i%M];
      }
    };
#endif
		
  }
}

#endif
