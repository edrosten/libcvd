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
#include "cvd/colourspace.h"
#include "cvd/byte.h"
#include "cvd/rgb.h"



namespace CVD
{
	
	template<> Image<Rgb<byte> > convert_image(const BasicImage<bayer>& from)
	{
		Image<Rgb<byte> > ret(from.size());

		ColourSpace::bayer_to_rgb(reinterpret_cast<const unsigned char*>(from.data()), 
								  reinterpret_cast<unsigned char*>(ret.data()), 
								  from.size().x, from.size().y);

		return ret;
	}

	template<> Image<byte> convert_image(const BasicImage<bayer>& from)
	{
		Image<byte> ret(from.size());

		ColourSpace::bayer_to_grey(reinterpret_cast<const unsigned char*>(from.data()), 
								  reinterpret_cast<unsigned char*>(ret.data()), 
								  from.size().x, from.size().y);

		return ret;
	}
	
	template<> Image<Rgb<byte> > convert_image(const BasicImage<yuv411>& from)
	{
		Image<Rgb<byte> > ret(from.size());

		ColourSpace::yuv411_to_rgb(reinterpret_cast<const unsigned char*>(from.data()),
								 from.totalsize(),
		                         reinterpret_cast<unsigned char*>(ret.data()));

		return ret;
	}

	template<> Image<byte> convert_image(const BasicImage<yuv411>& from)
	{
		Image<byte> ret(from.size());

		ColourSpace::yuv411_to_y(reinterpret_cast<const unsigned char*>(from.data()),
								 from.totalsize(),
		                         reinterpret_cast<unsigned char*>(ret.data()));

		return ret;
	}

}
