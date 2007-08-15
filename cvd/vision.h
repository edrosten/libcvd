/*
        This file is part of the CVD Library.

        Copyright (C) 2005 The Authors

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

        You should have received a copy of the GNU Lesser General Public
        License along with this library; if not, write to the Free Software
        Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef CVD_VISION_H_
#define CVD_VISION_H_

#include <vector>

#include <cvd/config.h>
#include <cvd/exceptions.h>
#include <cvd/image.h>
#include <cvd/internal/pixel_operations.h>
#include <cvd/utility.h>


#if defined(CVD_HAVE_TOON)
#include <TooN/TooN.h>
#include <TooN/helpers.h>
#endif

#if defined( __GNUC__) && defined(CVD_HAVE_SSE2)
#ifndef __SSE2__
#warning CVD was configured with SSE2 but SSE2 is not enabled on this compile (-msse2); expect breakage.
#endif
#include <emmintrin.h>
#endif

namespace CVD {

namespace Exceptions {

    /// %Exceptions specific to vision algorithms
    /// @ingroup gException
    namespace Vision {
        /// Base class for all Image_IO exceptions
        /// @ingroup gException
        struct All: public CVD::Exceptions::All {};

        /// Input images have incompatible dimensions
        /// @ingroup gException
        struct IncompatibleImageSizes : public All {
            IncompatibleImageSizes(const std::string & function)
            {
                what = "Incompatible image sizes in " + function;
            };
        };

        /// Input ImageRef not within image dimensions
        /// @ingroup gException
        struct ImageRefNotInImage : public All {
            ImageRefNotInImage(const std::string & function)
            {
                what = "Input ImageRefs not in image in " + function;
            };
        };
    };
};

/// subsamples an image to half its size by averaging 2x2 pixel blocks
/// @param in input image
/// @param out output image, must have the right dimensions versus input image
/// @throw IncompatibleImageSizes if out does not have half the dimensions of in
/// @ingroup gVision
template <class T>
void halfSample(const BasicImage<T>& in, BasicImage<T>& out)
{
    typedef typename Pixel::traits<T>::wider_type sum_type;
    if( (in.size()/2) != out.size())
        throw Exceptions::Vision::IncompatibleImageSizes("halfSample");
    const T* top = in.data();
    const T* bottom = top + in.size().x;
    const T* end = top + in.totalsize();
    int ow = out.size().x;
    int skip = in.size().x + (in.size().x % 2);
    T* p = out.data();
    while (bottom < end) {      
      for (int j=0; j<ow; j++) {
	*p = static_cast<T>((sum_type(top[0]) + top[1] + bottom[0] + bottom[1])/4);
	p++;
	top += 2;
	bottom += 2;
      }
      top += skip;
      bottom += skip;
    }
}

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
 void halfSample(const BasicImage<byte>& in, BasicImage<byte>& out);
#endif

/// subsamples an image to half its size by averaging 2x2 pixel blocks
/// @param in input image
/// @return The output image
/// @throw IncompatibleImageSizes if out does not have half the dimensions of in
/// @ingroup gVision
template <class T>
inline Image<T> halfSample(const BasicImage<T>& in)
{
	Image<T> out(in.size()/2);
	halfSample(in, out);
	return out;
}

/// subsamples an image repeatedly by half its size by averaging 2x2 pixel blocks.
/// This version will not create a copy for 0 octaves because it receives already
/// an Image and will reuse the data.
/// @param in input image
/// @param octaves number of halfsamplings 
/// @return The output image
/// @throw IncompatibleImageSizes if out does not have half the dimensions of in
/// @ingroup gVision
template <class T>
inline Image<T> halfSample( Image<T> in, unsigned int octaves){
    for( ;octaves > 0; --octaves){
        in = halfSample(in);
    }
    return in;
}

/// thresholds an image by setting all pixel values below a minimum to 0 and all values above to a given maximum
/// @param im input image changed in place
/// @param minimum threshold value
/// @param hi maximum value for values above the threshold
/// @ingroup gVision
template <class T>
void threshold(BasicImage<T>& im, const T& minimum, const T& hi)
{
  typename BasicImage<T>::iterator it = im.begin();
  typename BasicImage<T>::iterator end = im.end();
  while (it != end) {
    if (*it < minimum)
      *it = T();
    else
      *it = hi;
    ++it;
  }
}

/// computes mean and stddev of intensities in an image. These are computed for each component of the
/// pixel type, therefore the output are two pixels with mean and stddev for each component.
/// @param im input image
/// @param mean pixel element containing the mean of intensities in the image for each component
/// @param stddev pixel element containing the standard deviation for each component
/// @ingroup gVision
template <class T>
void stats(const BasicImage<T>& im, T& mean, T& stddev)
{
    const int c = Pixel::Component<T>::count;
    double v;
    double sum[c] = {0};
    double sumSq[c] = {0};
    const T* p = im.data();
    const T* end = im.data()+im.totalsize();
    while (p != end) {
        for (int k=0; k<c; k++) {
            v = Pixel::Component<T>::get(*p, k);
            sum[k] += v;
            sumSq[k] += v*v;
        }
        ++p;
    }
    for (int k=0; k<c; k++) {
        double m = sum[k]/im.totalsize();
        Pixel::Component<T>::get(mean,k) = (typename Pixel::Component<T>::type)m;
        sumSq[k] /= im.totalsize();
        Pixel::Component<T>::get(stddev,k) = (typename Pixel::Component<T>::type)sqrt(sumSq[k] - m*m);
    }
}

/// a functor multiplying pixels with constant value.
/// @ingroup gVision
template <class T>
struct multiplyBy
{
  const T& factor;
  multiplyBy(const T& f) : factor(f) {};
  template <class S> inline S operator()(const S& s) const {
    return s * factor;
  };
};

template <class S, class T, int Sn=Pixel::Component<S>::count, int Tn=Pixel::Component<T>::count> struct Gradient;
template <class S, class T> struct Gradient<S,T,1,2> {
  typedef typename Pixel::Component<S>::type SComp;
  typedef typename Pixel::Component<T>::type TComp;
  typedef typename Pixel::traits<SComp>::wider_type diff_type;
  static void gradient(const BasicImage<S>& I, BasicImage<T>& grad) {
    int w = I.size().x;
    typename BasicImage<S>::const_iterator s = I.begin() + w + 1;
    typename BasicImage<S>::const_iterator end = I.end() - w - 1;
    typename BasicImage<T>::iterator t = grad.begin() + w + 1;
    while (s != end) {
      Pixel::Component<T>::get(*t, 0) = Pixel::scalar_convert<TComp,SComp,diff_type>(diff_type(*(s+1)) - *(s-1));
      Pixel::Component<T>::get(*t, 1) = Pixel::scalar_convert<TComp,SComp,diff_type>(diff_type(*(s+w)) - *(s-w));
      s++;
      t++;
    }
    zeroBorders(grad);
  }
};

/// computes the gradient image from an image. The gradient image contains two components per pixel holding
/// the x and y components of the gradient.
/// @param im input image
/// @param out output image, must have the same dimensions as input image
/// @throw IncompatibleImageSizes if out does not have same dimensions as im
/// @ingroup gVision
template <class S, class T> void gradient(const BasicImage<S>& im, BasicImage<T>& out)
{
  if( im.size() != out.size())
    throw Exceptions::Vision::IncompatibleImageSizes("gradient");
  Gradient<S,T>::gradient(im,out);
}

#if defined(CVD_HAVE_SSE2) && defined(CVD_HAVE_EMMINTRIN)

 void gradient(const byte* in, short (*out)[2], int w, int h);

 inline void gradient(const BasicImage<byte>& im, BasicImage<short[2]>& out)
 {
     if( im.size() != out.size())
	 throw Exceptions::Vision::IncompatibleImageSizes("gradient");
     if (is_aligned<16>(im.data()) && is_aligned<16>(out.data()))
	 gradient(im.data(), out.data(), im.size().x, im.size().y);
     else
	 gradient<byte,short[2]>(im,out);
     zeroBorders(out);
 }

static inline __m128i zero_si128() { __m128i x; asm ( "pxor %0, %0  \n\t" : "=x"(x) ); return x; }

static inline __m128i shifter_load_si128(const void* p) { __m128i x; asm( "pshufd  $0xE4, (%1), %0 \n\t" : "=x"(x) : "r"(p) ); return x;}

template <class F> void gradient_threshold(const BasicImage<byte>& in, BasicImage<unsigned short>& magSq, int thresh, const F& f)
{
    const int w = in.size().x;
    const byte* curr = in.data() + w;
    unsigned short* out = magSq.data()+w;
    int threshSq = thresh*thresh;
    unsigned short reduced = (unsigned short)((threshSq&1)? threshSq/2 : threshSq/2-1);

    int candidates[w*2];
    int* c_last = candidates;
    int* c_curr = c_last+w;
    int last_count=0;
    int curr_count=0;
    
    __m128i t = _mm_set1_epi16(reduced);
    for (int i=1; i<in.size().y-1; i++) { 
	for (int j=0; j<w; j+=16, curr+=16, out+=16) {
	    __m128i hor_left, hor_right;
	    {
		__m128i hor = _mm_load_si128((const __m128i*)curr);
		hor_left = _mm_slli_si128(hor, 1);
		hor_right = _mm_srli_si128(hor, 1);
	    }
	    __m128i  hdiff, vdiff, down, up;
	    {
		up = _mm_load_si128((const __m128i*)(curr-w));
		down = _mm_load_si128((const __m128i*)(curr+w));

		__m128i zero = zero_si128();
		__m128i left = _mm_unpacklo_epi8(hor_right, zero);
		__m128i right = _mm_unpacklo_epi8(hor_left, zero);
		hdiff = _mm_insert_epi16(_mm_sub_epi16(right, left), short(curr[1]) - short(curr[-1]), 0);
		
		vdiff = _mm_sub_epi16(_mm_unpacklo_epi8(down, zero), _mm_unpacklo_epi8(up, zero));
		
		hdiff = _mm_mullo_epi16(hdiff,hdiff);
		vdiff = _mm_mullo_epi16(vdiff,vdiff);
		
		hor_right = _mm_unpackhi_epi8(hor_right, zero);
		hor_left = _mm_unpackhi_epi8(hor_left, zero);
		down = _mm_unpackhi_epi8(down, zero);
		up = _mm_unpackhi_epi8(up, zero);
	    }
	    __m128i first = _mm_avg_epu16(hdiff, vdiff);

	    hdiff = _mm_insert_epi16(_mm_sub_epi16(hor_left,hor_right), short(curr[16]) - short(curr[14]), 7);
	    vdiff = _mm_sub_epi16(down,up);
	    hdiff = _mm_mullo_epi16(hdiff,hdiff);
	    vdiff = _mm_mullo_epi16(vdiff,vdiff);

	    __m128i second = _mm_avg_epu16(hdiff, vdiff);
	    _mm_stream_si128((__m128i*)out, first);
	    _mm_stream_si128((__m128i*)(out+8), second);
	    first = _mm_cmpgt_epi16(first,t);
	    second = _mm_cmpgt_epi16(second,t);	    
	    int mask1 = _mm_movemask_epi8(first);
	    int mask2 = _mm_movemask_epi8(second);
	    if (mask1) {
		if (mask1&0x0002) c_curr[curr_count++]=j;
		if (mask1&0x0008) c_curr[curr_count++]=j+1;
		if (mask1&0x0020) c_curr[curr_count++]=j+2;
		if (mask1&0x0080) c_curr[curr_count++]=j+3;
		if (mask1&0x0200) c_curr[curr_count++]=j+4;
		if (mask1&0x0800) c_curr[curr_count++]=j+5;
		if (mask1&0x2000) c_curr[curr_count++]=j+6;
		if (mask1&0x8000) c_curr[curr_count++]=j+7;
	    }
	    if (mask2) {
		if (mask2&0x0002) c_curr[curr_count++]=j+8;
		if (mask2&0x0008) c_curr[curr_count++]=j+9;
		if (mask2&0x0020) c_curr[curr_count++]=j+10;
		if (mask2&0x0080) c_curr[curr_count++]=j+11;
		if (mask2&0x0200) c_curr[curr_count++]=j+12;
		if (mask2&0x0800) c_curr[curr_count++]=j+13;
		if (mask2&0x2000) c_curr[curr_count++]=j+14;
		if (mask2&0x8000) c_curr[curr_count++]=j+15;
	    }
	}	
	if (last_count)
	    f(i-1, c_last, last_count, out-3*w);
	std::swap(c_curr, c_last);
	last_count = curr_count;
	curr_count = 0;
    }
    zeroBorders(magSq);
}

#endif



template <class T, class S> inline void sample(const BasicImage<S>& im, double x, double y, T& result)
{
  int lx = (int)x;
  int ly = (int)y;
  x -= lx;
  y -= ly;
  for(unsigned int i = 0; i < Pixel::Component<T>::count; i++){
    Pixel::Component<T>::get(result,i) = static_cast<typename Pixel::Component<T>::type>(
        (1-y)*((1-x)*Pixel::Component<S>::get(im[ly][lx],i) + x*Pixel::Component<S>::get(im[ly][lx+1], i)) +
          y * ((1-x)*Pixel::Component<S>::get(im[ly+1][lx],i) + x*Pixel::Component<S>::get(im[ly+1][lx+1],i)));
  }
 }

template <class T, class S> inline T sample(const BasicImage<S>& im, double x, double y){
    T result;
    sample( im, x, y, result);
    return result;
}

inline void sample(const BasicImage<float>& im, double x, double y, float& result)
  {
    int lx = (int)x;
    int ly = (int)y;
    int w = im.size().x;
    const float* base = im[ly]+lx;
    float a = base[0];
    float b = base[1];
    float c = base[w];
    float d = base[w+1];
    float e = a-b;
    x-=lx;
    y-=ly;
    result = x*(y*(e-c+d)-e)+y*(c-a)+a;
  }

#if defined (CVD_HAVE_TOON)

/**
  * a normal member taking two arguments and returning an integer value.
  * @param in a image containing the information to be extracted.
  * @param out the image to be filled.  The whole image out image is filled by the in image.
  * @param M the matrix used to map point in the out matrix to those in the in matrix
  * @param inOrig origin in the in image
  * @param outOrig origin in the out image
  * @return the number of pixels not in the in image 
  * @Note: this will collide with transform in the std namespace
  */
template <class T> 
int transform(const BasicImage<T>& in, BasicImage<T>& out, const TooN::Matrix<2>& M, const TooN::Vector<2>& inOrig, const TooN::Vector<2>& outOrig)
{
   int i,j;
   const int w = out.size().x, h = out.size().y, iw = in.size().x, ih = in.size().y; 
   TooN::Vector<2> base; 
   TooN::Vector<2> p;

   TooN::Vector<2> across = M.T()[0];
   TooN::Vector<2> down =   M.T()[1];
   
   //min and max x and y
   base[0] = std::min( across[0]*(-outOrig[0]), across[0]*(-outOrig[0]+(double)w) ) +           
             std::min( down[0]*(-outOrig[1]), down[0]*(-outOrig[1]+(double)h) ) + inOrig[0];
   base[1] = std::min( across[1]*(-outOrig[0]), across[1]*(-outOrig[0]+(double)w) ) +           
             std::min( down[1]*(-outOrig[1]), down[1]*(-outOrig[1]+(double)h) ) + inOrig[1];
   p[0]    = std::max( across[0]*(-outOrig[0]), across[0]*(-outOrig[0]+(double)w) ) +           
             std::max( down[0]*(-outOrig[1]), down[0]*(-outOrig[1]+(double)h) ) + inOrig[0];
   p[1]    = std::max( across[1]*(-outOrig[0]), across[1]*(-outOrig[0]+(double)w) ) +           
             std::max( down[1]*(-outOrig[1]), down[1]*(-outOrig[1]+(double)h) ) + inOrig[1];

   

   //If the patch being extracted is completely in the image then no 
   //check is needed with each point.
   if ( p[0] < iw-1 && p[1] < ih-1 && base[0] >=0 && base[1] >=0 )
   {
      base = M * -outOrig  + inOrig;
      for (j=0;j<h;++j,base+=down) 
      {
         p = base;    
         for (i=0;i<w;++i,p+=across) 
           sample(in,p[0],p[1],out[j][i]);
      }
   } else {
      int tmp = 0;
	  base = M * -outOrig  + inOrig;
      for (j=0;j<h;++j,base+=down) 
      {
         p = base;    
         for (i=0;i<w;++i, p+=across) 
		 {
            //Make sure that we are extracting pixels in the image
            if ( p[0] < 0 ||  p[1] < 0 || p[0] >= iw-1 ||  p[1] >= ih-1)
            {
               zeroPixel(out[j][i]);
			   tmp++;
            } else
               sample(in,p[0],p[1],out[j][i]);
         }
      }
	  return tmp;
   }
   return 0;
}

  template <class T>  void transform(const BasicImage<T>& in, BasicImage<T>& out, const TooN::Matrix<3>& Minv /* <-- takes points in "out" to points in "in" */)
  {
    TooN::Vector<3> base = Minv.T()[2];
    TooN::Vector<2> offset;
    offset[0] = in.size().x/2;
    offset[1] = in.size().y/2;
    offset -= TooN::project(base);
    TooN::Vector<3> across = Minv.T()[0];
    TooN::Vector<3> down = Minv.T()[1];
    double w = in.size().x-1;
    double h = in.size().y-1;
    int ow = out.size().x;
    int oh = out.size().y;
    base -= down*(oh/2) + across*(ow/2);
    for (int row = 0; row < oh; row++, base+=down) {
      TooN::Vector<3> x = base;
      for (int col = 0; col < ow; col++, x += across) {
	TooN::Vector<2> p = project(x) + offset;
	if (p[0] >= 0 && p[0] <= w-1 && p[1] >=0 && p[1] <= h-1)
	  sample(in,p[0],p[1], out[row][col]);
	else
	  zeroPixel(out[row][col]);
      }
    }
  }
#endif

/// flips an image vertically in place.
template <class T> void flipVertical( Image<T> & in )
{
  int w = in.size().x;
  std::auto_ptr<T> buffer_auto(new T[w]);
  T* buffer = buffer_auto.get();
  T * top = in.data();
  T * bottom = top + (in.size().y - 1)*w;
  while( top < bottom )
  {
    std::copy(top, top+w, buffer);
    std::copy(bottom, bottom+w, top);
    std::copy(buffer, buffer+w, bottom);
    top += w;
    bottom -= w;
  }
}


//template<class T>


}; // namespace CVD

#endif // CVD_VISION_H_
