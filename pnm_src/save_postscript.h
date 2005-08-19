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
#ifndef PNM_SAVE_POSTSCRIPT_H
#define PNM_SAVE_POSTSCRIPT_H

#include <iostream>
#include <string>
#include <cvd/internal/disk_image.h>

namespace CVD
{
namespace PNM
{

	class ps_out: public CVD::Image_IO::image_out
	{
		public:
			ps_out(std::ostream&, int  xsize, int ysize, int ch, bool use2bytes, const std::string&c="");

			virtual void 	write_raw_pixel_lines(const unsigned char*, unsigned long);
			virtual void 	write_raw_pixel_lines(const unsigned short*, unsigned long);
			virtual ~ps_out();

		protected:
			std::string bytes_to_base85(int n);
			void output_header();
			int lines;
			unsigned char buf[4];
			int  num_in_buf;
			ps_out(std::ostream&);
			std::ostream& 	o;
	};

	class eps_out: public ps_out
	{
		public:
			virtual ~eps_out();
			eps_out(std::ostream&, int  xsize, int ysize, int ch, bool use2bytes, const std::string&c="");
	};

}
}
#endif
