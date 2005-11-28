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
    51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301  USA
*/
#ifndef CVD_RGB_TRAITS_H
#define CVD_RGB_TRAITS_H

#include <cvd/rgb.h>
#include <cvd/rgba.h>
#include <cvd/rgb8.h>
#include <cvd/internal/builtin_components.h>

namespace CVD
{
	namespace Pixel
	{
		
		template<class P> struct Component<Rgb<P> >
		{
			typedef P type;
			static const unsigned int count = 3;

			
			//This version is much faster, with -funroll-loops
			static const P& get(const Rgb<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<const P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}

			static P& get(Rgb<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<P*>(&pixel)+i);
				// return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}
		};

		template<> struct Component<Rgb8>
		{
			typedef unsigned char type;
			static const unsigned int count = 3;

			static const type& get(const Rgb8& pixel, unsigned int i)
			{
				return *(reinterpret_cast<const unsigned char*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}

			static type& get(Rgb8& pixel, unsigned int i)
			{
				return *(reinterpret_cast<unsigned char*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}
		};

		template<class P> struct Component<Rgba<P> >
		{
			typedef P type;
			static const unsigned int count = 4;

			static const P& get(const Rgba<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<const P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : (i==2 ?pixel.blue: pixel.alpha));
			}

			static P& get(Rgba<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : (i==2 ?pixel.blue: pixel.alpha));
			}
		};


	}
}
#endif
