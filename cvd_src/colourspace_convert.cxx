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
#include <cvd/colourspace.h>
#include <cvd/colourspace_convert.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>

namespace CVD
{
	
  template<> void convert_image(const BasicImage<bayer_bggr>& from, BasicImage<Rgb<byte> >& to)
  {
    ColourSpace::bayer_to_rgb_bggr(reinterpret_cast<const unsigned char*>(from.data()), 
			      reinterpret_cast<unsigned char*>(to.data()), 
			      from.size().x, from.size().y);
  }

  template<> void convert_image(const BasicImage<bayer_gbrg>& from, BasicImage<Rgb<byte> >& to)
  {
    ColourSpace::bayer_to_rgb_gbrg(reinterpret_cast<const unsigned char*>(from.data()),
			      reinterpret_cast<unsigned char*>(to.data()), 
			      from.size().x, from.size().y);
  }

  template<> void convert_image(const BasicImage<bayer_grbg>& from, BasicImage<Rgb<byte> >& to)
  {
    ColourSpace::bayer_to_rgb_grbg(reinterpret_cast<const unsigned char*>(from.data()),
			      reinterpret_cast<unsigned char*>(to.data()), 
			      from.size().x, from.size().y);
  }
  
  template<> void convert_image(const BasicImage<bayer_rggb>& from, BasicImage<Rgb<byte> >& to)
  {
    ColourSpace::bayer_to_rgb_rggb(reinterpret_cast<const unsigned char*>(from.data()),
			      reinterpret_cast<unsigned char*>(to.data()), 
			      from.size().x, from.size().y);
  }

  template<> void convert_image(const BasicImage<bayer_bggr>& from, BasicImage<byte>& to)
  {
    ColourSpace::bayer_to_grey_bggr(reinterpret_cast<const unsigned char*>(from.data()), to.data(), from.size().x, from.size().y);
  }
  
  template<> void convert_image(const BasicImage<bayer_gbrg>& from, BasicImage<byte>& to)
  {
    ColourSpace::bayer_to_grey_gbrg(reinterpret_cast<const unsigned char*>(from.data()), to.data(), from.size().x, from.size().y);
  }
  
  template<> void convert_image(const BasicImage<bayer_grbg>& from, BasicImage<byte>& to)
  {
    ColourSpace::bayer_to_grey_grbg(reinterpret_cast<const unsigned char*>(from.data()), to.data(), from.size().x, from.size().y);
  }
	
  template<> void convert_image(const BasicImage<bayer_rggb>& from, BasicImage<byte>& to)
  {
    ColourSpace::bayer_to_grey_rggb(reinterpret_cast<const unsigned char*>(from.data()), to.data(), from.size().x, from.size().y);
  }
  
  template<> void convert_image(const BasicImage<yuv411>& from, BasicImage<Rgb<byte> >& to)
  {
    ColourSpace::yuv411_to_rgb(reinterpret_cast<const unsigned char*>(from.data()),
			       from.totalsize(),
			       reinterpret_cast<unsigned char*>(to.data()));
  }

  template<> void convert_image(const BasicImage<yuv411>& from, BasicImage<byte>& to)
  {
    ColourSpace::yuv411_to_y(reinterpret_cast<const unsigned char*>(from.data()),
			     from.totalsize(),
			     reinterpret_cast<unsigned char*>(to.data()));
  }

  template<> void convert_image(const BasicImage<yuv422>& from, BasicImage<Rgb<byte> >& to)
  {
    ColourSpace::yuv422_to_rgb(reinterpret_cast<const byte*>(from.data()), reinterpret_cast<byte*>(to.data()), from.size().x, from.size().y);
  }

  template<> void convert_image(const BasicImage<yuv422>& from, BasicImage<byte>& to)
  {
    ColourSpace::yuv422_to_grey(reinterpret_cast<const byte*>(from.data()), to.data(), from.size().x, from.size().y);
  }

  // Name changed from 'convert_image' to prevent conflict with previous convert_image
  // with same method signature.
  template<> std::pair<Image<byte>,Image<Rgb<byte> > > convert_image_pair(const BasicImage<yuv411>& from)
  {
    Image<byte> rety(from.size());
    Image<Rgb<byte> > retc(from.size());

    ColourSpace::yuv411_to_rgb_y(reinterpret_cast<const unsigned char*>(from.data()),
				 from.totalsize(),
				 reinterpret_cast<unsigned char*>(retc.data()),
				 reinterpret_cast<unsigned char*>(rety.data()));

    return std::pair<Image<byte>,Image<Rgb<byte> > >(rety, retc);

  }
	
	template<> void convert_image(const BasicImage<vuy422>& from, BasicImage<Rgb<byte> >& to)
	{
		ColourSpace::vuy422_to_rgb(reinterpret_cast<const byte*>(from.data()), reinterpret_cast<byte*>(to.data()), from.size().x, from.size().y);
	}

	template<> void convert_image(const BasicImage<vuy422>& from, BasicImage<byte>& to)
	{
		ColourSpace::vuy422_to_grey(reinterpret_cast<const byte*>(from.data()), to.data(), from.size().x, from.size().y);
	}	
}
