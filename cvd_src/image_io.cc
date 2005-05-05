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
#include "cvd/image_io.h"
#include "cvd/internal/disk_image.h"
#include "pnm_src/pnm_grok.h"

#include "cvd/config.h"

#ifdef CVD_IMAGE_HAVE_JPEG
	#include "pnm_src/jpeg.h"
#endif

#ifdef CVD_IMAGE_HAVE_TIFF
	#include "pnm_src/tiff.h"
#endif
#include <sstream>

using namespace CVD;
using namespace std;

Exceptions::Image_IO::ImageSizeMismatch::ImageSizeMismatch(const ImageRef& src, const ImageRef& dest)
{
	ostringstream o;
	o <<
"Image load: Size mismatch when loading an image (size " << src << ") in to a non\
resizable image (size " << dest << ").";

	what = o.str();
}

Exceptions::Image_IO::MalformedImage::MalformedImage(const string& why)
{
	what = "Image input: " + why;
}

Exceptions::Image_IO::UnsupportedImageType::UnsupportedImageType()
{
	what = "Image input: Unsuppported image type.";
}

Exceptions::Image_IO::EofBeforeImage::EofBeforeImage()
{
	what = "Image input: End of file occured before image.";
}

Exceptions::Image_IO::WriteError::WriteError(const string& s)
{
	what = "Error writing " + s;
}

Exceptions::Image_IO::ReadTypeMismatch::ReadTypeMismatch(const bool read8)
{
	what = string("Image input: Attempting to read ") + (read8?"8":"16") + "bit data from " + (read8?"16":"8")  + "bit file (probably an internal error).";
}

Exceptions::Image_IO::UnseekableIstream::UnseekableIstream(const string& s)
{
	what = "Image input: Loading " + s + " images requires seekable istream.";
}

Exceptions::Image_IO::UnsupportedImageSubType::UnsupportedImageSubType(const string& i, const string& why)
{
	what = "Image input: Unsupported subtype of " + i+ " image: " + why;
}

Exceptions::Image_IO::InternalLibraryError::InternalLibraryError(const std::string& l, const std::string e)
{
	what = "Internal error in " + l + " library: " + e;
}
namespace CVD
{
namespace Image_IO
{
/*
long image_base::elements_per_line() const
{
	if(m_is_rgb)
		return xs * 3;
	else
		return xs;
}

long image_base::x_size() const
{
	return xs;
}

long image_base::y_size() const
{
	return ys;
}

bool image_base::is_2_byte() const
{
	return m_is_2_byte;
}

bool image_base::is_rgb() const
{
	return m_is_rgb;
}
*/
image_in::~image_in(){}
image_out::~image_out(){}


image_out* image_factory::out(std::ostream& o, long xsize, long ysize, ImageType::ImageType t, int try_channels, bool try_2byte, const std::string& c)
{
	switch(t)
	{
		case ImageType::PNM:
			return new CVD::PNM::pnm_out(o, xsize, ysize, try_channels, try_2byte, c);
			break;

		#ifdef CVD_IMAGE_HAVE_JPEG
			case ImageType::JPEG:
				return new CVD::PNM::jpeg_out(o, xsize, ysize, try_channels, try_2byte, c);
				break;
		#endif

		default:
			throw Exceptions::Image_IO::UnsupportedImageType();
			return NULL;
	}
}

image_in* image_factory::in(std::istream& i)
{
	unsigned char c = i.get();

	if(i.eof())
		throw Exceptions::Image_IO::EofBeforeImage();

	i.putback(c);

	if(c == 'P')
		return new CVD::PNM::pnm_in(i);
	#ifdef CVD_IMAGE_HAVE_JPEG
		else if(c == 0xff)
			return new CVD::PNM::jpeg_in(i);
	#endif
	#ifdef CVD_IMAGE_HAVE_TIFF
		else if(c == 'I')
			return new CVD::PNM::tiff_in(i);
	#endif
	else
		throw Exceptions::Image_IO::UnsupportedImageType();

}


}
}



