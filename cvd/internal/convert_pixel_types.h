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
#ifndef CVD_CONVERT_PIXEL_TYPES_H
#define CVD_CONVERT_PIXEL_TYPES_H

#include <math.h>

#include <cvd/abs.h>
#include <cvd/internal/scalar_convert.h>
#include <cvd/internal/builtin_components.h>
#include <cvd/internal/rgb_components.h>
#include <cvd/internal/is_pod.h>
#include <limits>

namespace CVD{namespace Pixel
{
  //All "conversion" classes must have a "void convert_pixel(from, to)"
  //nonstatic templated member.
  
  //This conversion knows how to convert from any size pixel to any other.
  
  template <class From, class To, int CF=Pixel::Component<From>::count, int CT=Pixel::Component<To>::count> struct GenericConversion;

  template <class From, class To> struct GenericConversion<From,To,1,1> {
    static void convert(const From& from, To& to) { 
      to = scalar_convert<To,From>::from(from);
    }
  };
  
  template <class From, class To, int N> struct GenericConversion<From,To,N,N> {
    typedef typename Pixel::Component<From>::type FromS;
    typedef typename Pixel::Component<To>::type ToS;
    static void convert(const From& from, To& to) { 
      for (int i=0; i<N; i++)
	Pixel::Component<To>::get(to,i) = scalar_convert<ToS,FromS>::from(Pixel::Component<From>::get(from,i));
    }
  };

  template <class T, int N> struct GenericConversion<T,T,N,N> { 
    static void convert(const T& from, T& to) { 
      to = from;
    }
  };

  template <class T, int N> struct GenericConversion<T[N],T[N], N, N> { 
    typedef T array[N];
    static void convert(const array& from, array& to) { 
      for (int i=0; i<N; i++)
	to[i] = from[i];
    }
  };

  template <class Rgbish, class Scalar> struct CIE {
    static void convert(const Rgbish& from, Scalar& to) {
      const double wr=0.299, wg=0.587, wb=0.114;
      to = scalar_convert<Scalar,double>::from(wr*scalar_convert<double,typename Pixel::Component<Rgbish>::type>::from(from.red) + 
					      wg*scalar_convert<double,typename Pixel::Component<Rgbish>::type>::from(from.green) + 
					      wb*scalar_convert<double,typename Pixel::Component<Rgbish>::type>::from(from.blue));
    }
  };

  template <class Rgbish, class Scalar> struct Uniform {
    static void convert(const Rgbish& from, Scalar& to) {
      typedef typename Pixel::Component<Rgbish>::type T;
      typename traits<T>::wider_type sum = Pixel::Component<Rgbish>::get(from,0);
      sum += Pixel::Component<Rgbish>::get(from,1);
      sum += Pixel::Component<Rgbish>::get(from,2);
      to = scalar_convert<Scalar,T>::from(static_cast<T>(sum/3));
    }
  };
  
  template <class Scalar, class Vec> struct Replicate {
    static void convert(const Scalar& from, Vec& to) {
      typedef typename Pixel::Component<Vec>::type T;
      Pixel::Component<Vec>::get(to,0) = scalar_convert<T, Scalar>::from(from);
      for (size_t i=1; i<Pixel::Component<Vec>::count; i++) 
	Pixel::Component<Vec>::get(to,i) = Pixel::Component<Vec>::get(to,0);
    }
  };
  
  template <class Scalar, class T> struct GreyToRgba {
    static void convert(const Scalar& from, Rgba<T>& to) {
      to.red = to.green = to.blue = scalar_convert<T, Scalar>::from(from);
      to.alpha = traits<T>::max_intensity;
    }
  };

  template <class A, class B> void RgbToRgb(const A& from, B& to) {
    typedef typename Pixel::Component<A>::type T;
    typedef typename Pixel::Component<B>::type S;
    to.red = scalar_convert<S,T>::from(from.red);
    to.green = scalar_convert<S,T>::from(from.green);
    to.blue = scalar_convert<S,T>::from(from.blue);
  }
  
  template <class A, class B> struct RgbishToRgbish {
    static void convert(const A& from, B& to) {
      RgbToRgb(from,to);
    }
  };
  
  template <class A,class T> struct RgbishToRgbish<A,Rgba<T> > {
    static void convert(const A& from, Rgba<T>& to) {
      RgbToRgb(from,to);
      to.alpha = traits<T>::max_intensity;
    }
  };

  template <class S,class T> struct RgbishToRgbish<Rgba<S>,Rgba<T> > {
    static void convert(const Rgba<S>& from, Rgba<T>& to) {
      RgbToRgb(from,to);
      to.alpha = scalar_convert<T,S>::from(from.alpha);
    }
  };

  // Default conversions

  template <class From, class To, int FN=Pixel::Component<From>::count, int TN=Pixel::Component<To>::count> struct DefaultConversion { 
    typedef GenericConversion<From,To> type; 
  };

  // Scalar to X
  template <class S, class T> struct DefaultConversion<S,Rgb<T>,1,3> { typedef Replicate<S,Rgb<T> > type; };
  template <class S> struct DefaultConversion<S,Rgb8,1,3> { typedef Replicate<S,Rgb8> type; };
  template <class S, class T> struct DefaultConversion<S,Rgba<T>,1,4> { typedef GreyToRgba<S,T> type; };

  // Rgb<T> to X
  template <class T, class S> struct DefaultConversion<Rgb<T>,S,3,1> { typedef CIE<Rgb<T>,S> type; };
  template <class T> struct DefaultConversion<Rgb<T>,Rgb8, 3,3> { typedef RgbishToRgbish<Rgb<T>, Rgb8> type; };
  template <class T, class S> struct DefaultConversion<Rgb<T>,Rgba<S>,3,4> { typedef RgbishToRgbish<Rgb<T>, Rgba<S> > type; };
  template <class T, class S> struct DefaultConversion<Rgb<T>,Rgb<S>,3,3> { typedef RgbishToRgbish<Rgb<T>, Rgb<S> > type; };

  // Rgb8 to X
  template <class S> struct DefaultConversion<Rgb8,S,3,1> { typedef CIE<Rgb8,S> type; };
  template <class S> struct DefaultConversion<Rgb8,Rgb<S>,3,3> { typedef RgbishToRgbish<Rgb8, Rgb<S> > type; };
  template <class S> struct DefaultConversion<Rgb8,Rgba<S>,3,4> { typedef RgbishToRgbish<Rgb8, Rgba<S> > type; };

  // Rgba<T> to X
  template <class T, class S> struct DefaultConversion<Rgba<T>,S,4,1> { typedef CIE<Rgba<T>,S> type; };
  template <class T, class S> struct DefaultConversion<Rgba<T>,Rgb<S>,4,3> { typedef RgbishToRgbish<Rgba<T>, Rgb<S> > type; };
  template <class T> struct DefaultConversion<Rgba<T>,Rgb8,4,3> { typedef RgbishToRgbish<Rgba<T>, Rgb8> type; };
  template <class T, class S> struct DefaultConversion<Rgba<T>,Rgba<S>,4,4> { typedef RgbishToRgbish<Rgba<T>, Rgba<S> > type; };

  
  template <class From, class To, class Conv=typename DefaultConversion<From,To>::type, 
    bool both_pod=CVD::Internal::is_POD<From>::is_pod && CVD::Internal::is_POD<To>::is_pod> struct ConvertPixels {
    static inline void convert(const From* from, To* to, size_t count) {
      for (size_t i=0; i<count; i++) 
	Conv::convert(from[i], to[i]);
    }
  };

  template <class T> struct ConvertPixels<T,T,GenericConversion<T,T>,true> {
    static inline void convert(const T* from, T* to, size_t count) {
      memcpy(to, from, count*sizeof(T));
    }
  };
  
}}
#endif
