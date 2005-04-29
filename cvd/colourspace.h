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

		
		/// Convert Bayer pattern of the form ??? to greyscale data
		/// @param bggr The input data
		/// @param grey The output data
		/// @param width The width of the image
		/// @param height The height of the image
		void bayer_to_grey(unsigned char* bggr, unsigned char* grey, unsigned int width, unsigned int height);

		/// Convert Bayer pattern of the form ??? to rgb444 data
		/// @param bggr The input data
		/// @param grey The output data
		/// @param width The width of the image
		/// @param height The height of the image
		void bayer_to_rgb(unsigned char* bggr, unsigned char* rgb, unsigned int width, unsigned int height);

		
	}
}

#endif
