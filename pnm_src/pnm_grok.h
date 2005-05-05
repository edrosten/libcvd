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
#ifndef PNM_GROK_H
#define PNM_GROK_H

#include <iostream>
#include <string>
#include <cvd/internal/disk_image.h>

namespace CVD
{
namespace PNM
{

	class pnm_in: public CVD::Image_IO::image_in
	{
		public:
			pnm_in(std::istream&);
			virtual void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
			virtual void get_raw_pixel_lines(unsigned short*, unsigned long nlines);

			
		private:
			std::istream&	i;
			bool 	is_text;
			int     type, maxval;
			int     lines_so_far;
			void	read_header();
			bool    can_proc_lines(unsigned long);
	};

	class pnm_out: public CVD::Image_IO::image_out
	{
		public:
			pnm_out(std::ostream&, int  xsize, int ysize, int ch, bool use2bytes, const std::string&c="");

			virtual void 	write_raw_pixel_lines(const unsigned char*, unsigned long);
			virtual void 	write_raw_pixel_lines(const unsigned short*, unsigned long);
			
		private:
			
			std::ostream& 	o;
			int     type, maxval;
			int     lines_so_far;
			bool    is_text;
			void	read_header();
			bool    can_proc_lines(unsigned long);
			void	write_header(const std::string&);
	};

}
}
#endif
