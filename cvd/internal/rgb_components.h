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
#include <cvd/internal/pixel_traits.h>

namespace CVD
{
	namespace Pixel
	{
		
		template<class P> struct Component<Rgb<P> >
		{
			typedef P type;
			static const size_t count = 3;

			
			//This version is much faster, with -funroll-loops
			static const P& get(const Rgb<P>& pixel, size_t i)
			{
				return *(reinterpret_cast<const P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}

			static P& get(Rgb<P>& pixel, size_t i)
			{
				return *(reinterpret_cast<P*>(&pixel)+i);
				// return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}
		};

		template<> struct Component<Rgb8>
		{
			typedef unsigned char type;
			static const size_t count = 3;

			static const type& get(const Rgb8& pixel, size_t i)
			{
				return *(reinterpret_cast<const unsigned char*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}

			static type& get(Rgb8& pixel, size_t i)
			{
				return *(reinterpret_cast<unsigned char*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}
		};

		template<class P> struct Component<Rgba<P> >
		{
			typedef P type;
			static const size_t count = 4;

			static const P& get(const Rgba<P>& pixel, size_t i)
			{
				return *(reinterpret_cast<const P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : (i==2 ?pixel.blue: pixel.alpha));
			}

			static P& get(Rgba<P>& pixel, size_t i)
			{
				return *(reinterpret_cast<P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : (i==2 ?pixel.blue: pixel.alpha));
			}
		};

		template <class T> struct is_Rgb { enum { value = 0 }; };
		template <class T> struct is_Rgb<Rgb<T> > { enum { value = 1 }; };
		template <> struct is_Rgb<Rgb8> { enum { value = 1 }; };
		template <class T> struct is_Rgb<Rgba<T> > { enum { value = 1 }; };

		template<class T, int LIFT> struct traits<Rgb<T>, LIFT> 
		{ 
		  typedef Rgb<typename Pixel::traits<T>::wider_type> wider_type; 
		  typedef Rgb<typename Pixel::traits<T>::float_type> float_type;
		};
  
		template<int LIFT> struct traits<Rgb8, LIFT> 
		{ 
		  typedef Rgb<int> wider_type; 
		  typedef Rgb<float> float_type;
		};

	}
		template <class T> struct Rgb_ops {
		  template <class S> static inline T sum(const T& a, const S& b) { return T(a.red+b.red, a.green+b.green, a.blue+b.blue); }
		  template <class S> static inline void add(T& a, const S& b) { a.red+=b.red; a.green+=b.green; a.blue+=b.blue; }
		  template <class S> static inline T diff(const T& a, const S& b) { return T(a.red-b.red, a.green-b.green, a.blue-b.blue); }
		  template <class S> static inline void sub(T& a, const S& b) { a.red-=b.red; a.green-=b.green; a.blue-=b.blue; }
		  template <class S> static inline T prod(const T& a, const S& b) { return T(a.red*b, a.green*b, a.blue*b); }
		  template <class S> static inline void mul(T& a, const S& b) { a.red*=b; a.green*=b; a.blue*=b; }
		  template <class S> static inline T quot(const T& a, const S& b) { return T(a.red/b, a.green/b, a.blue/b); }
		  template <class S> static inline void div(T& a, const S& b) { a.red/=b; a.green/=b; a.blue/=b; }
		  template <class S> static inline void assign(T& a, const S& b) { a.red=b.red; a.green=b.green; a.blue=b.blue; }
		};

		template <class T, class S> inline Rgb<T> operator+(const Rgb<T>& a, const Rgb<S>& b) { return Rgb_ops<Rgb<T> >::sum(a,b); }
		template <class T, class S> inline Rgb<T>& operator+=(Rgb<T>& a, const Rgb<S>& b) { Rgb_ops<Rgb<T> >::add(a,b); return a; }
		template <class T, class S> inline Rgb<T> operator-(const Rgb<T>& a, const Rgb<S>& b) { return Rgb_ops<Rgb<T> >::diff(a,b); }
		template <class T, class S> inline Rgb<T>& operator-=(Rgb<T>& a, const Rgb<S>& b) { Rgb_ops<Rgb<T> >::sub(a,b); return a; }
		template <class T, class S> inline Rgb<T> operator*(const Rgb<T>& a, const S& b) { return Rgb_ops<Rgb<T> >::prod(a,b); }
		template <class T, class S> inline Rgb<T> operator*(const S& b, const Rgb<T>& a) { return Rgb_ops<Rgb<T> >::prod(a,b); }
		template <class T, class S> inline Rgb<T>& operator*=(Rgb<T>& a, const S& b) { Rgb_ops<Rgb<T> >::mul(a,b); return a; }
		template <class T, class S> inline Rgb<T> operator/(const Rgb<T>& a, const S& b) { return Rgb_ops<Rgb<T> >::quot(a,b); }
		template <class T, class S> inline Rgb<T> operator/(const S& b, const Rgb<T>& a) { return Rgb_ops<Rgb<T> >::quot(a,b); }
		template <class T, class S> inline Rgb<T>& operator/=(Rgb<T>& a, const S& b) { Rgb_ops<Rgb<T> >::div(a,b); return a; }

		
	
}
#endif
