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
