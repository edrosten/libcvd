#include "cvd/config.h"
#include "cvd/colourspace.h"
#include "cvd/utility.h"
#include "cvd/internal/assembly.h"

namespace CVD {
  namespace ColourSpace {
    inline void yuv420p_to_rgb_c(const unsigned char* y, const unsigned char* u, const unsigned char* v, unsigned char* rgb, unsigned int width, unsigned int height)
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

#if CVD_INTERNAL_HAVE_YUV420P_MMX
    extern "C"{
	void cvd_asm_yuv420p_to_rgb(const unsigned char*, unsigned char*, int);
    }

#endif

    void yuv420p_to_rgb(const unsigned char* y, const unsigned char* u, const unsigned char* v, unsigned char* rgb, unsigned int width, unsigned int height) {
#if CVD_INTERNAL_HAVE_YUV420P_MMX
      if (is_aligned<8>(y) && (u == y + width*height) && is_aligned<8>(u) && (v == u + width*height/4) && is_aligned<8>(v) &&
	  is_aligned<8>(rgb) && ((width&4) == 0) && ((height&1)==0))
	cvd_asm_yuv420p_to_rgb(y,rgb,width,height);
      else
	yuv420p_to_rgb_c(y, u, v, rgb, width, height);
#else
      yuv420p_to_rgb_c(y, u, v, rgb, width, height);
#endif
    }    
    
    void yuv420p_to_grey(const unsigned char* y, const unsigned char*, const unsigned char*, 
			 unsigned char* grey, unsigned int width, unsigned int height)
    {
      memcpy(grey, y, width*height);
    }
    
    
  }
}
