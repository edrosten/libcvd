#include "cvd/utility.h"
#include "cvd/colourspace.h"
#include "cvd_src/yuv422.h"

namespace CVD {
namespace ColourSpace {
    
	void yuv422_to_rgb(const unsigned char* yuv, unsigned char* rgb, unsigned int width, unsigned int height) 
	{
		yuv422_to_rgb_c(yuv, rgb, width*height);
	}    

	void yuv422_to_grey(const unsigned char* yuv, unsigned char* grey, unsigned int width, unsigned int height)
	{
		yuv422_to_grey_c(yuv, grey, width*height);
	}    
}}
