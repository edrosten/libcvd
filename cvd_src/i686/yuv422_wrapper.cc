#include "cvd/utility.h"
#include "cvd/colourspace.h"
#include "cvd_src/yuv422.h"

namespace CVD {
namespace ColourSpace {
	extern "C"
	{
		void cvd_asm_yuv422_to_rgb(const unsigned char* yuv, unsigned char* rgb, unsigned int width);
		void cvd_asm_yuv422_to_grey(const unsigned char* grey, unsigned char* rgb, unsigned int width);
	}

	void yuv422_to_rgb(const unsigned char* yuv, unsigned char* rgb, unsigned int width, unsigned int height) 
	{
		if (is_aligned<8>(yuv) && is_aligned<8>(rgb)) 
		{
			cvd_asm_yuv422_to_rgb(yuv,rgb,width*height);
			int remaining = (width*height)&7;
			if (remaining) 
			{
				int done = (width*height) & (~0x7);
				yuv422_to_rgb_c(yuv+done*2, rgb+done*3, remaining);	  
			}	  
		}
		else
			yuv422_to_rgb_c(yuv, rgb, width*height);
	}    

	void yuv422_to_grey(const unsigned char* yuv, unsigned char* grey, unsigned int width, unsigned int height)
	{
		if (is_aligned<8>(yuv) && is_aligned<8>(grey)) 
		{
			cvd_asm_yuv422_to_grey(yuv,grey,width*height);
			int remaining = (width*height)&7;
			if (remaining) 
			{
				int done = (width*height) & (~0x7);
				yuv422_to_grey_c(yuv+done*2, grey+done, remaining);
			}	  
		}
		else
			yuv422_to_grey_c(yuv, grey, width*height);
	}    
}
}
