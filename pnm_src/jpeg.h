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
#ifndef PNM_JPEG_H
#define PNM_JPEG_H

#include <iostream>
#include <string>
#include <cvd/internal/disk_image.h>

extern "C"{
#include <jpeglib.h>
};

namespace CVD
{
namespace PNM
{

	class jpeg_in: public CVD::Image_IO::image_in
	{
		public:
			jpeg_in(std::istream&);
			virtual void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
			virtual void get_raw_pixel_lines(unsigned short*, unsigned long nlines);
			virtual ~jpeg_in();
			
		private:
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			std::istream&	i;
	};

	class jpeg_out: public CVD::Image_IO::image_out
	{
		public:
			jpeg_out(std::ostream&, int  xsize, int ysize, int ch, bool use2bytes, const std::string&c="");

			virtual void 	write_raw_pixel_lines(const unsigned char*, unsigned long);
			virtual void 	write_raw_pixel_lines(const unsigned short*, unsigned long);
			virtual ~jpeg_out();
			
		private:
			struct jpeg_compress_struct cinfo;
			struct jpeg_error_mgr jerr;
			std::ostream& 	o;
	};

}
}
#endif
