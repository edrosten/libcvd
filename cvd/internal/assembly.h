#ifndef CVD_ASSEMBLY_H
#define CVD_ASSEMBLY_H

#include <sys/types.h>

namespace CVD{ 
  namespace Internal {  
    namespace Assembly {
      extern "C" {
	void rgb_to_gray(const unsigned char* in, unsigned char* out, int size, int wr, int wg, int wb);
	void yuv420p_to_rgb(const unsigned char* yuv, unsigned char* rgb, int width, int height);
	void yuv422_to_rgb(const unsigned char* yuv, unsigned char* rgb, int size);
	void yuv422_to_grey(const unsigned char* yuv, unsigned char* gray, int size);
	
	void halfsample(const unsigned char* in, unsigned char* out, int width, int height);
	
	void byte_to_short_difference(const unsigned char* a, const unsigned char* b, short* c, unsigned int size);
	void short_difference(const short* a, const short* b, short* c, unsigned int size);
	void int_difference(const int32_t* a, const int32_t* b, int32_t* c, unsigned int size);
	void float_difference(const float* a, const float* b, float* c, unsigned int size);
	
	void float_assign_mul(const float* a, float f, float* c, unsigned long size);
	void float_add_mul_add(const float* a, const float* b, float f, float* c, unsigned long size);
	void float_add_mul_add_unaligned(const float* a, const float* b, float f, float* c, unsigned long size);
	
	float float_innerproduct(const float* a, const float* b, int w, int h, int stride_a, int stride_b);
      }
    }
  }
}

#endif
