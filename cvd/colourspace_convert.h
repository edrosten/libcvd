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
#include <cvd/image_convert_fwd.h>
#include <cvd/colourspaces.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/image_convert.h>


namespace CVD
{
		
	/// Convert Bayer pattern of various forms to greyscale data
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<bayer_bggr>& from, BasicImage<byte>& to);
	template<> void convert_image(const BasicImage<bayer_grbg>& from, BasicImage<byte>& to);
	template<> void convert_image(const BasicImage<bayer_gbrg>& from, BasicImage<byte>& to);
	template<> void convert_image(const BasicImage<bayer_rggb>& from, BasicImage<byte>& to);
	
	/// Convert Bayer pattern of various forms to rgb data
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<bayer_bggr>& from, BasicImage<Rgb<byte> >& to);
	template<> void convert_image(const BasicImage<bayer_grbg>& from, BasicImage<Rgb<byte> >& to);
	template<> void convert_image(const BasicImage<bayer_gbrg>& from, BasicImage<Rgb<byte> >& to);
	template<> void convert_image(const BasicImage<bayer_rggb>& from, BasicImage<Rgb<byte> >& to);
	 
	
	/// Convert YUV 411 pixel data to RGB
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<yuv411>& from, BasicImage<Rgb<byte> >& to);


	/// Convert YUV 411 pixel data to Y only
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<yuv411>& from, BasicImage<byte>& to);
	
	/// Convert YUV 422 pixel data to RGB
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<yuv422>& from, BasicImage<Rgb<byte> >& to);
	  

	/// Convert YUV 422 pixel data to Y only
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<yuv422>& from, BasicImage<byte>& to);

	// Name changed from 'convert_image' to prevent conflict with previous convert_image
	// with same method signature.

	/// Convert YUV 411 pixel data to both Y and RGB
	/// @param from The input data
	/// @ingroup gImageIO
	template<> std::pair<Image<byte>,Image<Rgb<byte> > > convert_image_pair(const BasicImage<yuv411>& from);
	
	
	/// Convert VUY 422 pixel data to RGB
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<vuy422>& from, BasicImage<Rgb<byte> >& to);
	
	/// Convert VUY 422 pixel data to Y only
	/// @param from The input data
	/// @param to The output data
	/// @ingroup gImageIO
	template<> void convert_image(const BasicImage<vuy422>& from, BasicImage<byte>& to);	
}
