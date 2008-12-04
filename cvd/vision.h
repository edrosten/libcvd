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
#include <memory>

#include <cvd/exceptions.h>
#include <cvd/image.h>
#include <cvd/internal/pixel_operations.h>
#include <cvd/utility.h>


#if defined(CVD_HAVE_TOON)
#include <TooN/TooN.h>
#include <TooN/helpers.h>
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

void halfSample(const BasicImage<byte>& in, BasicImage<byte>& out);

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


#ifndef DOXYGEN_IGNORE_INTERNAL
inline void gradient(const BasicImage<byte>& im, BasicImage<short[2]>& out);
#endif


template <class T, class S> inline void sample(const BasicImage<S>& im, double x, double y, T& result)
{
  typedef typename Pixel::Component<S>::type SComp;
  typedef typename Pixel::Component<T>::type TComp;
  int lx = (int)x;
  int ly = (int)y;
  x -= lx;
  y -= ly;
  for(unsigned int i = 0; i < Pixel::Component<T>::count; i++){
    Pixel::Component<T>::get(result,i) = Pixel::scalar_convert<TComp,SComp>(
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
    result = (float)(x*(y*(e-c+d)-e)+y*(c-a)+a);
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
template <class T, class S>
int transform(const BasicImage<S>& in, BasicImage<T>& out, const TooN::Matrix<2>& M, const TooN::Vector<2>& inOrig, const TooN::Vector<2>& outOrig, const T defaultValue = T())
{
    const int w = out.size().x, h = out.size().y, iw = in.size().x, ih = in.size().y; 
    const TooN::Vector<2> across = M.T()[0];
    const TooN::Vector<2> down =   M.T()[1];
   
    const TooN::Vector<2> p0 = inOrig - M*outOrig;
    const TooN::Vector<2> p1 = p0 + w*across;
    const TooN::Vector<2> p2 = p0 + h*down;
    const TooN::Vector<2> p3 = p0 + w*across + h*down;
        
    // ul --> p0
    // ur --> w*across + p0
    // ll --> h*down + p0
    // lr --> w*across + h*down + p0
    double min_x = p0[0], min_y = p0[1];
    double max_x = min_x, max_y = min_y;
   
    // Minimal comparisons needed to determine bounds
    if (across[0] < 0)
	min_x += w*across[0];
    else
	max_x += w*across[0];
    if (down[0] < 0)
	min_x += h*down[0];
    else
	max_x += h*down[0];
    if (across[1] < 0)
	min_y += w*across[1];
    else
	max_y += w*across[1];
    if (down[1] < 0)
	min_y += h*down[1];
    else
	max_y += h*down[1];
   
    // This gets from the end of one row to the beginning of the next
    const TooN::Vector<2> carriage_return = down - w*across;

    //If the patch being extracted is completely in the image then no 
    //check is needed with each point.
    if (min_x >= 0 && min_y >= 0 && max_x < iw-1 && max_y < ih-1) 
    {
	TooN::Vector<2> p = p0;
	for (int i=0; i<h; ++i, p+=carriage_return)
	    for (int j=0; j<w; ++j, p+=across) 
		sample(in,p[0],p[1],out[i][j]);
	return 0;
    } 
    else // Check each source location
    {
	// Store as doubles to avoid conversion cost for comparison
	const double x_bound = iw-1;
	const double y_bound = ih-1;
	int count = 0;
	TooN::Vector<2> p = p0;
	for (int i=0; i<h; ++i, p+=carriage_return) {
	    for (int j=0; j<w; ++j, p+=across) {
		//Make sure that we are extracting pixels in the image
		if (0 <= p[0] && 0 <= p[1] &&  p[0] < x_bound && p[1] < y_bound)
		    sample(in,p[0],p[1],out[i][j]);
		else {
            out[i][j] = defaultValue;
		    ++count;
		}
	    }
	}
	return count;
    }
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


namespace median {
    template <class T> inline T median3(T a, T b, T c) {
	if (b<a)
	    return std::max(b,std::min(a,c));
	else
	    return std::max(a,std::min(b,c));	
    }
    
    template <class T> inline void sort3(T& a, T& b, T& c) {
	using std::swap;
	if (b<a) swap(a,b);
	if (c<b) swap(b,c);
	if (b<a) swap(a,b);
    }
    
    template <class T> T median_3x3(const T* p, const int w) {
	T a = p[-w-1], b = p[-w], c = p[-w+1], d=p[-1], e=p[0], f=p[1], g=p[w-1], h=p[w], i=p[w+1];
	sort3(a,b,c);
	sort3(d,e,f);
	sort3(g,h,i);
	e = median3(b,e,h);
	g = std::max(std::max(a,d),g);
	c = std::min(c,std::min(f,i));
	return median3(c,e,g);
    }
    
    template <class T> void median_filter_3x3(const T* p, const int w, const int n, T* out)
    {
	T a = p[-w-1], b = p[-w], d=p[-1], e=p[0], g=p[w-1], h=p[w];
	sort3(a,d,g);
	sort3(b,e,h);
	for (int j=0; j<n; ++j, ++p, ++out) {
	    T c = p[-w+1], f = p[1], i = p[w+1];
	    sort3(c,f,i);
	    *out = median3(std::min(std::min(g,h),i), 
			   median3(d,e,f), 
			   std::max(std::max(a,b),c));
	    a=b; b=c; d=e; e=f; g=h; h=i;
	}
    }
}

    template <class T> void median_filter_3x3(const SubImage<T>& I, SubImage<T> out)
    {
	assert(out.size() == I.size());
	const int s = I.row_stride();
	const int n = I.size().x - 2;
	for (int i=1; i+1<I.size().y; ++i)
	    median::median_filter_3x3(I[i]+1, s, n, out[i]+1);
    }

void median_filter_3x3(const SubImage<byte>& I, SubImage<byte> out);

//template<class T>


}; // namespace CVD

#endif // CVD_VISION_H_
