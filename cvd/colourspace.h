#ifndef CVD_COLOURSPACE_H
#define CVD_COLOURSPACE_H

namespace CVD
{
	/// Internal colourspace conversions
	namespace ColourSpace
	{
		/// Convert YUV 411 pixel data to RGB
		/// @param yuv411 The input data
		/// @param npix The number of pixels
		/// @param out The output data
		void yuv411_to_rgb(const unsigned char* yuv411, int npix, unsigned char* out);
		/// Convert YUV 411 pixel data to Y only
		/// @param yuv411 The input data
		/// @param npix The number of pixels
		/// @param out The output data
		void yuv411_to_y(const unsigned char* yuv411, int npix, unsigned char* out);
	}
}

#endif
