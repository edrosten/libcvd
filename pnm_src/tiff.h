#ifndef PNM_TIFF
#define PNM_TIFF

#include <iostream>
#include <string>
#include <tiffio.h>
#include <cvd/internal/disk_image.h>
#include <stdarg.h>

namespace CVD
{
namespace PNM
{

	class tiff_in: public CVD::Image_IO::image_in
	{
		public:
			tiff_in(std::istream&);
			virtual void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
			virtual void get_raw_pixel_lines(unsigned short*, unsigned long nlines);
			virtual ~tiff_in();
			
		private:
			TIFF* tif;
			std::streamoff start, length;
			std::istream&	i;
			unsigned int row;
			
			bool use_cooked_rgba_interface;
			bool inverted_grey;
			uint32 *raster_data;
			
			static tsize_t write(thandle_t vis, tdata_t data, tsize_t count);
			static tsize_t read(thandle_t vis, tdata_t data, tsize_t count);
			static toff_t seek(thandle_t vis, toff_t off, int dir);
			static toff_t size(thandle_t vis);
			static int close(thandle_t vis);
			static int map(thandle_t, tdata_t*, toff_t*);
			static void unmap(thandle_t, tdata_t, toff_t);


	};
/*
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
*/
}
}
#endif
