#include "cvd/colourspace.h"

namespace CVD {
	namespace ColourSpace {
		void yuv420p_to_rgb_c(const unsigned char* y, const unsigned char* u, const unsigned char* v, unsigned char* rgb, unsigned int width, unsigned int height);
		void yuv420p_to_rgb(const unsigned char* y, const unsigned char* u, const unsigned char* v, unsigned char* rgb, unsigned int width, unsigned int height) {
				yuv420p_to_rgb_c(y, u, v, rgb, width, height);
		}    
	}
}
