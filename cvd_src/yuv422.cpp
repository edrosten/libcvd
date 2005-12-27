#include <cvd/config.h>
#include <cvd/internal/assembly.h>
#include <cvd/utility.h>
#include <cvd/colourspace.h>

namespace CVD {
  namespace ColourSpace {
    
    inline void yuv422_to_rgb_c(const unsigned char* yuv, unsigned char* rgb, unsigned int total)
    {
      int yy, uu, vv, ug_plus_vg, ub, vr;
      int r,g,b;
      //rgb += width*(height-1)*3;
      total /= 2;
      while (total--) {
	yy = yuv[0] << 8;
	uu = yuv[1] - 128;
	vv = yuv[3] - 128;
	ug_plus_vg = uu * 88 + vv * 183;
	ub = uu * 454;
	vr = vv * 359;
	r = (yy + vr) >> 8;
	g = (yy - ug_plus_vg) >> 8;
	b = (yy + ub) >> 8;
	rgb[0] = r < 0 ? 0 : (r > 255 ? 255 : (unsigned char)r);
	rgb[1] = g < 0 ? 0 : (g > 255 ? 255 : (unsigned char)g);
	rgb[2] = b < 0 ? 0 : (b > 255 ? 255 : (unsigned char)b);
	yy = yuv[2] << 8;
	r = (yy + vr) >> 8;
	g = (yy - ug_plus_vg) >> 8;
	b = (yy + ub) >> 8;
	rgb[3] = r < 0 ? 0 : (r > 255 ? 255 : (unsigned char)r);
	rgb[4] = g < 0 ? 0 : (g > 255 ? 255 : (unsigned char)g);
	rgb[5] = b < 0 ? 0 : (b > 255 ? 255 : (unsigned char)b);
	yuv += 4;
	rgb += 6;
      }
    }

    void yuv422_to_rgb(const unsigned char* yuv, unsigned char* rgb, unsigned int width, unsigned int height) {
#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
      if (is_aligned<8>(yuv) && is_aligned<8>(rgb)) {
	Internal::Assembly::yuv422_to_rgb(yuv,rgb,width*height);
	int remaining = (width*height)&7;
	if (remaining) {
	  int done = (width*height) & (~0x7);
	  yuv422_to_rgb_c(yuv+done*2, rgb+done*3, remaining);	  
	}	  
      }
      else
	yuv422_to_rgb_c(yuv, rgb, width*height);
#else
      yuv422_to_rgb_c(yuv, rgb, width*height);
#endif
    }    
   
    inline void yuv422_to_grey_c(const unsigned char* yuv, unsigned char* grey, unsigned int total)
    {
      total/=2;
      while (total--) {
	grey[0] = yuv[0];
	grey[1] = yuv[2];
	yuv += 4;
	grey += 2;
      }
    }

    void yuv422_to_grey(const unsigned char* yuv, unsigned char* grey, unsigned int width, unsigned int height) {
#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
      if (is_aligned<8>(yuv) && is_aligned<8>(grey)) {
	Internal::Assembly::yuv422_to_grey(yuv,grey,width*height);
	int remaining = (width*height)&7;
	if (remaining) {
	  int done = (width*height) & (~0x7);
	  yuv422_to_grey_c(yuv+done*2, grey+done, remaining);
	}	  
      }
      else
	yuv422_to_grey_c(yuv, grey, width*height);
#else
      yuv422_to_grey_c(yuv, grey, width*height);
#endif
    }    

	void vuy422_to_rgb(const unsigned char* yuv, unsigned char* rgb, unsigned int width, unsigned int height)
	{
		int yy, uu, vv, ug_plus_vg, ub, vr;
		int r,g,b;
		unsigned int total = width*height;
		total /= 2;
		while (total--) {
			yy = yuv[1] << 8;
			uu = yuv[0] - 128;
			vv = yuv[2] - 128;
			ug_plus_vg = uu * 88 + vv * 183;
			ub = uu * 454;
			vr = vv * 359;
			r = (yy + vr) >> 8;
			g = (yy - ug_plus_vg) >> 8;
			b = (yy + ub) >> 8;
			rgb[0] = r < 0 ? 0 : (r > 255 ? 255 : (unsigned char)r);
			rgb[1] = g < 0 ? 0 : (g > 255 ? 255 : (unsigned char)g);
			rgb[2] = b < 0 ? 0 : (b > 255 ? 255 : (unsigned char)b);
			yy = yuv[3] << 8;
			r = (yy + vr) >> 8;
			g = (yy - ug_plus_vg) >> 8;
			b = (yy + ub) >> 8;
			rgb[3] = r < 0 ? 0 : (r > 255 ? 255 : (unsigned char)r);
			rgb[4] = g < 0 ? 0 : (g > 255 ? 255 : (unsigned char)g);
			rgb[5] = b < 0 ? 0 : (b > 255 ? 255 : (unsigned char)b);
			yuv += 4;
			rgb += 6;
		}
	}
	
	void vuy422_to_grey(const unsigned char* yuv, unsigned char* grey, unsigned int width, unsigned int height)
	{
		unsigned int total = width*height;
		total/=2;
		while (total--) {
			grey[0] = yuv[1];
			grey[1] = yuv[3];
			yuv += 4;
			grey += 2;
		}
	}
  }
}
