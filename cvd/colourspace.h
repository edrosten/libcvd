#ifndef CVD_COLOURSPACE_H
#define CVD_COLOURSPACE_H

namespace CVD
{
	namespace ColourSpace
	{
		void yuv411_to_rgb(const unsigned char* yuv411, int npix, unsigned char* out);
		void yuv411_to_y(const unsigned char* yuv411, int npix, unsigned char* out);
	}
}

#endif
