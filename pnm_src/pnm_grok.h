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
