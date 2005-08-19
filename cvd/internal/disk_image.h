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
#ifndef DISK_IMAGE_IN_H
#define DISK_IMAGE_IN_H

#include <iostream>
#include <string>

#include <cvd/config.h>

namespace CVD
{	
	// The Doxygen documentation for this is in image_io.h, which has a copy of this
	// with the macros expanded, which is only included when making documentation.
	namespace ImageType
	{
		enum ImageType
		{
			PNM,
			PS,
			EPS,
			CVD_IMAGE_HAVE_JPEG  //This is a macro, ending in ,
		};
	}

	namespace Image_IO
	{

		class image_base
		{
			public:
				bool is_2_byte()const {return m_is_2_byte;}
				int channels(){return m_channels;}
				//bool is_rgb() const {return m_is_rgb;}
				long  x_size() const {return xs;}
				long  y_size() const {return ys;}
				long  elements_per_line() const {return xs * m_channels;}
				
			protected:
				long	xs, ys;
				bool 	m_is_2_byte;
				int		m_channels;
		};

		class image_in : public image_base
		{
			public:
				virtual void get_raw_pixel_lines(unsigned char*, unsigned long nlines)=0;
				virtual void get_raw_pixel_lines(unsigned short*, unsigned long nlines)=0;

				virtual ~image_in();

		};

		class image_out: public image_base
		{
			public:
				virtual void write_raw_pixel_lines(const unsigned char*, unsigned long nlines)=0;
				virtual void write_raw_pixel_lines(const unsigned short*, unsigned long nlines)=0;
				virtual ~image_out();
		};
		
		class image_factory
		{
			public:
				static image_in* in(std::istream&);
				static image_out* out(std::ostream&, long xsize, long ysize, ImageType::ImageType type,
									 int try_channels, bool try_2byte, const std::string& c);
		};
	}

}




#endif
