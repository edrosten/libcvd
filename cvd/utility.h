#ifndef CVD_UTILITY_H
#define CVD_UTILITY_H

#include <cvd/image.h>
#include <cvd/internal/is_pod.h>
#include <cvd/internal/pixel_traits.h>
#include <cvd/internal/convert_pixel_types.h>

namespace CVD { //begin namespace

  /// generic image copy function for copying sub rectangles of images into other images.
  /// @param in input image to copy from
  /// @param out output image to copy into
  /// @param size size of the area to copy
  /// @param begin upper left corner of the area to copy, by default the upper left corner of the input image
  /// @param dst upper left corner of the destination in the output image, by default the upper left corner of the output image
  /// @throw ImageRefNotInImage if either begin is not in the input image or dst not in the output image
  /// @ingroup gConvert
  template<class S, class T> void copy(const BasicImage<S>& in, BasicImage<T>& out, ImageRef size=ImageRef(-1,-1), ImageRef begin = ImageRef(), ImageRef dst = ImageRef())
  {
    if (size.x == -1 && size.y == -1)
      size = in.size();
    // FIXME: This should be an exception, but which one do I use? 
    // I refuse to define another "ImageRefNotInImage" in this namespace.
    if (!(in.in_image(begin) && out.in_image(dst) && in.in_image(begin+size - ImageRef(1,1)) && out.in_image(dst+size - ImageRef(1,1)))){	
	std::cerr << "bad copy: " << in.size() << " " << out.size() << " " << size << " " << begin << " " << dst << std::endl;
	int *p = 0;
	*p = 1;
    }
    if (in.size() == out.size() && size == in.size() && begin == ImageRef() && dst == ImageRef()) {
      Pixel::ConvertPixels<S,T>::convert(in.data(), out.data(), in.totalsize());
      return;
    }
    
    const S* from = &in[begin];
    T* to = &out[dst];
    int i = 0;
    while (i++<size.y) {
      Pixel::ConvertPixels<S,T>::convert(from, to, size.x);
      from += in.size().x;
      to += out.size().x;
    }
  }
  
  template <class T, bool pod = Internal::is_POD<T>::is_pod> struct ZeroPixels {
    static void zero(T* pixels, int count) {
      while (count--) {
	*(pixels++) = T();
      }
    }
  };

  template <class T> struct ZeroPixels<T,true> {
    static void zero(T* pixels, int count) {
      memset(pixels, 0, sizeof(T)*count);
    }
  };

  template <class T> void zeroPixel(T& pixel) {
    ZeroPixels<T>::zero(&pixel, 1);
  }

  template <class T> void zeroPixels(T* pixels, int count) {
    ZeroPixels<T>::zero(pixels, count);
  }

  template <class T> void zeroBorders(BasicImage<T>& I)
  {
    if (I.size().y == 0)
      return;
    zeroPixels(I[0], I.size().x);
    for (int r=0;r<I.size().y-1; r++)
      zeroPixels(I[r]+I.size().x-1,2);
    zeroPixels(I[I.size().y-1], I.size().x);
  }

  template <class A, class B> inline void differences(const A* a, const A* b, B* diff, unsigned int size) {
    for (unsigned int j=0; j<size; j++)
      *(diff++) = *(b++)-*(a++);
  }
  
#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
  void differences(const unsigned char* a, const unsigned char* b, short* diff, unsigned int size);
  void differences(const short* a, const short* b, short* diff, unsigned int size);
  void differences(const int32_t* a, const int32_t* b, int32_t* diff, unsigned int size);
#endif 
  
#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
  void differences(const float* a, const float* b, float* diff, unsigned int size);
#endif

  template <class T,class S> inline void add_mul_add(const T* a, const T* b, const S& f, T* c, size_t count)
  {
    for (size_t i=0; i<count; i++)
      c[i] += f * (a[i] + b[i]);
  }

  template <class T,class S> inline void add_mul_add(const Rgb<T>* a, const Rgb<T>* b, const S& f, Rgb<T>* c, size_t count)
  {
    add_mul_add(reinterpret_cast<const T*>(a), reinterpret_cast<const T*>(b), f, reinterpret_cast<T*>(c), count*3);
  }

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
  void add_mul_add(const float* a, const float* b, const float& f, float* c, size_t count);
#endif

  template <class T,class S> inline void assign_mul(const T* in, const S& f, T* out, size_t count)
  {
    for (size_t i=0; i<count; i++)
      out[i] = f * in[i];
  }
  
#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
  inline void assign_mul(const float* in, const double& f, float* out, size_t count) {
    assign_mul(in,(float)f,out,count);
  }
  void assign_mul(const float* in, const float& f, float* out, size_t count);
#endif

  template<int bytes> bool is_aligned(const void* ptr);

  template<> inline bool is_aligned<8>(const void* ptr) {   return ((reinterpret_cast<size_t>(ptr)) & 0x7) == 0;   }
  template<> inline bool is_aligned<16>(const void* ptr) {  return ((reinterpret_cast<size_t>(ptr)) & 0xF) == 0;   }

}

#endif
