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
#include "cvd/image_io.h"
#include "cvd/internal/load_and_save.h"

#include "cvd/config.h"

#include <sstream>
#include <fstream>
#include <cstring>

using namespace std;
namespace CVD{

Exceptions::Image_IO::ImageSizeMismatch::ImageSizeMismatch(const ImageRef& src, const ImageRef& dest)
{
	ostringstream o;
	o <<
"Image load: Size mismatch when loading an image (size " << src << ") in to a non\
resizable image (size " << dest << ").";

	what = o.str();
}

Exceptions::Image_IO::OpenError::OpenError(const string& name, const string& why, int error)
{
	what = "Opening file: " + name+ " (" + why + "): " + strerror(errno);
}

Exceptions::Image_IO::MalformedImage::MalformedImage(const string& why)
{
	what = "Image input: " + why;
}

Exceptions::Image_IO::UnsupportedImageType::UnsupportedImageType()
{
	what = "Image input: Unsuppported image type.";
}

Exceptions::Image_IO::IfstreamNotOpen::IfstreamNotOpen()
{
	what = "Image input: File stream has not been opened succesfully.";
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

ImageType::ImageType string_to_image_type(const std::string& name)
{ 
	size_t dot = name.rfind('.');
	if (dot == std::string::npos)
		return  ImageType::PNM;
	
	std::string suffix = name.substr(dot+1,name.length()-dot-1);
	for (size_t i=0; i<suffix.length(); i++)
		suffix[i] = tolower(suffix[i]);


	if (suffix == "ps") 
	        return  ImageType::PS;
#ifdef CVD_IMAGE_HAVE_JPEG
 	else if (suffix == "jpg") 
	         return  ImageType::JPEG;
#endif
	else if (suffix == "eps")
		return  ImageType::EPS;
	else if (suffix == "bmp") 
		return  ImageType::BMP;
	else if (suffix == "pnm" || suffix == "ppm" || suffix == "pgm" || suffix == "pbm") 
		return  ImageType::PNM;
	else
		return ImageType::Unknown;
}

}
