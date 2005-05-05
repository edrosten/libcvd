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
#include "cvd/image_convert_fwd.h"
#include "cvd/colourspaces.h"
#include "cvd/byte.h"
#include "cvd/rgb.h"



namespace CVD
{
		
	/// Convert Bayer pattern of the form ??? to greyscale data
	/// @param bggr The input data
	/// @param grey The output data
	/// @param width The width of the image
	/// @param height The height of the image
	/// @ingroup gImageIO
	template<> Image<byte> convert_image(const BasicImage<bayer>& from);	
	
	/// Convert Bayer pattern of the form ??? to rgb444 data
	/// @param bggr The input data
	/// @param grey The output data
	/// @param width The width of the image
	/// @param height The height of the image
	/// @ingroup gImageIO
	template<> Image<Rgb<byte> > convert_image(const BasicImage<bayer>& from);
	
	
	/// Convert YUV 411 pixel data to RGB
	/// @param yuv411 The input data
	/// @param npix The number of pixels
	/// @param out The output data
	/// @ingroup gImageIO
	template<> Image<Rgb<byte> > convert_image(const BasicImage<yuv411>& from);


	/// Convert YUV 411 pixel data to Y only
	/// @param yuv411 The input data
	/// @param npix The number of pixels
	/// @param out The output data
	/// @ingroup gImageIO
	template<> Image<byte> convert_image(const BasicImage<yuv411>& from);
}
