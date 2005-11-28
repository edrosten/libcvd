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
#ifndef CVD_IMAGE_CONVERT_H
#define CVD_IMAGE_CONVERT_H

#include <cvd/internal/convert_pixel_types.h>
#include <cvd/internal/rgb_components.h>
#include <cvd/image_convert_fwd.h>

namespace CVD
{

  // The most general case: one row at a time

  template <class From, class To, class Conv=typename Pixel::DefaultConversion<From,To>::type, int both_pod=Internal::is_POD<From>::is_pod && Internal::is_POD<To>::is_pod> struct ConvertImage {
    static void convert(const BasicImage<From>& from, BasicImage<To>& to) {
      for (int r=0; r<from.size().y; r++)
	Pixel::ConvertPixels<From,To>::convert(from[r], to[r], from.size().x);
    };
  };

  // The blat case: memcpy all data at once 
  template <class T> struct ConvertImage<T,T,Pixel::GenericConversion<T,T>,1> {
    static void convert(const BasicImage<T>& from, BasicImage<T>& to) {
      memcpy(to.data(), from.data(), from.totalsize() * sizeof(T));
    };
  };

  
  template<class D, class C, class Conv> void convert_image(const BasicImage<C>& from, BasicImage<D>& to)
  {
    if (from.size() != to.size())
      throw Exceptions::Image::IncompatibleImageSizes(__FUNCTION__);
    ConvertImage<C,D,Conv>::convert(from, to);
  }
  
  template<class D, class C> void convert_image(const BasicImage<C>& from, BasicImage<D>& to)
  {
    if (from.size() != to.size())
      throw Exceptions::Image::IncompatibleImageSizes(__FUNCTION__);
    ConvertImage<C,D>::convert(from, to);
  }

  /// Convert an image from one type to another using a specified conversion.
  /// @param D The destination image pixel type
  /// @param C The source image pixel type
  /// @param Conv The conversion to use
  /// @param from The image to convert from
  /// @ingroup gImageIO
  
  template<class D, class C, class Conv> Image<D> convert_image(const BasicImage<C>& from)
  {
    Image<D> to(from.size());
    ConvertImage<C,D,Conv>::convert(from, to);
    return to;
  }
    
  /// Convert an image from one type to another using the default.
  /// @param D The destination image pixel type
  /// @param C The source image pixel type
  /// @param from The image to convert from
  /// @ingroup gImageIO
  
  template<class D, class C> Image<D> convert_image(const BasicImage<C>& from)
  {
    Image<D> to(from.size());
    ConvertImage<C,D>::convert(from, to);
    return to;
  }
  
	
}

#endif
