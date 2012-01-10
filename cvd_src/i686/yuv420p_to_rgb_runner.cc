#include "cvd/colourspace.h"
#include "cvd/utility.h"

namespace CVD {
	namespace ColourSpace {

		inline void yuv420p_to_rgb_c(const unsigned char* y, const unsigned char* u, const unsigned char* v, unsigned char* rgb, unsigned int width, unsigned int height);

		extern "C"{
			void cvd_asm_yuv420p_to_rgb(const unsigned char*, unsigned char*, int, int);
		}


		void yuv420p_to_rgb(const unsigned char* y, const unsigned char* u, const unsigned char* v, unsigned char* rgb, unsigned int width, unsigned int height) {
			if (is_aligned<8>(y) && (u == y + width*height) && is_aligned<8>(u) && (v == u + width*height/4) && is_aligned<8>(v) &&
			                                                             is_aligned<8>(rgb) && ((width&4) == 0) && ((height&1)==0))
				cvd_asm_yuv420p_to_rgb(y,rgb,width,height);
			else
				yuv420p_to_rgb_c(y, u, v, rgb, width, height);
		}    
	}
}
