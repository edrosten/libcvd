#include <cvd/config.h>
#include <cvd/utility.h>
// internal functions used by CVD vision algorithm implementations
#include <cvd/internal/assembly.h>

using namespace std;

namespace CVD {


#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
  void differences(const unsigned char* a, const unsigned char* b, short* diff, unsigned int size)
  {
    if (!is_aligned<8>(a) || !is_aligned<8>(b) || !is_aligned<8>(diff)) {      
      unsigned int steps = std::min(8 - ((size_t)a&0x7), size);
      if (((size_t)(b+steps) & 0x7) || ((size_t)(diff+steps)&0x7)) {
	differences<unsigned char, short>(a,b,diff,size);
	return;
      }
      differences<unsigned char, short>(a,b,diff,steps);
      if (steps == size)
	return;
      a += steps;
      b += steps;
      diff += steps;
      size -= steps;
    }
    unsigned int block = size & (~0x7);
    Internal::Assembly::byte_to_short_difference(a,b,diff,block);
    if (size > block)
      differences<unsigned char, short>(a+block,b+block,diff+block,size-block);
  }
#endif
  

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
  void differences(const short* a, const short* b, short* diff, unsigned int size)
  {
    if (!is_aligned<8>(a) || !is_aligned<8>(b) || !is_aligned<8>(diff)) {
      unsigned int steps = std::min(4 - ((size_t)a&0x7)/2, size);
      if (((size_t)(b+steps) & 0x7) || ((size_t)(diff+steps)&0x7)) {
	differences<short, short>(a,b,diff,size);
	return;
      }
      differences<short, short>(a,b,diff,steps);
      if (steps == size)
	return;
      a += steps;
      b += steps;
      diff += steps;
      size -= steps;
    }
    unsigned int block = size & (~0x3);
    Internal::Assembly::short_difference(a,b,diff,block);
    if (size > block)
      differences<short, short>(a+block,b+block,diff+block,size-block);
  }
#endif

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
  void differences(const int32_t* a, const int32_t* b, int32_t* diff, unsigned int size)
  {
    if (!is_aligned<8>(a) || !is_aligned<8>(b) || !is_aligned<8>(diff)) {
      unsigned int steps = std::min(2 - ((size_t)a&0x7)/4, size);
      if (((size_t)(b+steps) & 0x7) || ((size_t)(diff+steps)&0x7)) {
	differences<int32_t, int32_t>(a,b,diff,size);
	return;
      }
      differences<int32_t, int32_t>(a,b,diff,steps);
      if (steps == size)
	return;
      a += steps;
      b += steps;
      diff += steps;
      size -= steps;
    }
    unsigned int block = size & (~0x1);
    Internal::Assembly::int_difference(a,b,diff,block);
    if (size > block)
      differences<int32_t, int32_t>(a+block,b+block,diff+block,size-block);
  }
#endif

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
  void differences(const float* a, const float* b, float* diff, unsigned int size)
  {
    if (!is_aligned<16>(a) || !is_aligned<16>(b) || !is_aligned<16>(diff)) {
      unsigned int steps = std::min(4 - ((size_t)a&0xF)/4, size);
      if (((size_t)(b+steps) & 0xF) || ((size_t)(diff+steps)&0xF)) {
	differences<float, float>(a,b,diff,size);
	return;
      }
      differences<float, float>(a,b,diff,steps);
      if (steps == size)
	return;
      a += steps;
      b += steps;
      diff += steps;
      size -= steps;
    }
    unsigned int block = size & (~0x3);
    Internal::Assembly::float_difference(a,b,diff,block);
    if (size > block)
      differences<float,float>(a+block,b+block,diff+block,size-block);
  }
#endif

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
  void add_mul_add(const float* a, const float* b, const float& f, float* c, size_t size)
  {
    if (!is_aligned<16>(a)) {
      size_t steps = std::min(4 - ((size_t)a&0xF)/4, size);
      add_mul_add<float>(a,b,f,c,steps);
      a += steps;
      b += steps;
      c += steps;
      size -= steps;
      if (size == 0)
	return;
    }
    size_t block = size & (~0x3);
    if (is_aligned<16>(a) && is_aligned<16>(b) && is_aligned<16>(c))
      Internal::Assembly::float_add_mul_add(a,b,f,c,block);
    else
      Internal::Assembly::float_add_mul_add_unaligned(a,b,f,c,block);
    if (size > block)
      add_mul_add<float>(a+block,b+block,f,c+block,size-block);      
  }
#endif

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
  void assign_mul(const float* in, const float& f, float* out, size_t size) {
    if (!is_aligned<16>(in)) {
      size_t steps = std::min(4 - ((size_t)in&0xF)/4, size);
      assign_mul<float>(in,f,out,steps);
      in += steps;
      out += steps;
      size -= steps;
      if (size == 0)
	return;
    }
    size_t block = size & (~0x3);
    if (is_aligned<16>(in) && is_aligned<16>(out))
      Internal::Assembly::float_assign_mul(in,f,out,block);
    else {
      assign_mul<float>(in,f,out,size);
      return;
    }
    if (size > block)
      assign_mul<float>(in+block,f,out+block,size-block);    
  }
#endif

}
