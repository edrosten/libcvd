#include "cvd/config.h"
#include "cvd/colourspace.h"
#include "cvd/utility.h"
#include "cvd/internal/assembly.h"

namespace CVD {
  namespace ColourSpace {
    void yuv420p_to_rgb_c(const unsigned char* y, const unsigned char* u, const unsigned char* v, unsigned char* rgb, unsigned int width, unsigned int height)
    {
      unsigned int halfwidth = width >> 1;
      int yy, uu, vv, ug_plus_vg, ub, vr;
      int r,g,b;
      unsigned int i,j;
      for (i=0; i<height; ++i) {
	for (j=0;j<halfwidth;++j) {
	  yy = y[0] << 8;
	  uu = *(u++) - 128;
	  vv = *(v++) - 128;
	  ug_plus_vg = uu * 88 + vv * 183;
	  ub = uu * 454;
	  vr = vv * 359;
	  r = (yy + vr) >> 8;
	  g = (yy - ug_plus_vg) >> 8;
	  b = (yy + ub) >> 8;
	  rgb[0] = r < 0 ? 0 : (r > 255 ? 255 : (unsigned char)r);
	  rgb[1] = g < 0 ? 0 : (g > 255 ? 255 : (unsigned char)g);
	  rgb[2] = b < 0 ? 0 : (b > 255 ? 255 : (unsigned char)b);
	  yy = y[1] << 8;
	  r = (yy + vr) >> 8;
	  g = (yy - ug_plus_vg) >> 8;
	  b = (yy + ub) >> 8;
	  rgb[3] = r < 0 ? 0 : (r > 255 ? 255 : (unsigned char)r);
	  rgb[4] = g < 0 ? 0 : (g > 255 ? 255 : (unsigned char)g);
	  rgb[5] = b < 0 ? 0 : (b > 255 ? 255 : (unsigned char)b);
	  y += 2;
	  rgb += 6;
	}
	if ((i&1) == 0) {
	  u -= halfwidth;
	  v -= halfwidth;
	}
      }
    }

    void yuv420p_to_grey(const unsigned char* y, const unsigned char*, const unsigned char*, 
			 unsigned char* grey, unsigned int width, unsigned int height)
    {
      memcpy(grey, y, width*height);
    }
  }
}
