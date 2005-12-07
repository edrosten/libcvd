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
        for (unsigned int k=0; k<c; k++) {
            v = Pixel::Component<T>::get(*p, k);
            sum[k] += v;
            sumSq[k] += v*v;
        }
        ++p;
    }
    for (unsigned int k=0; k<c; k++) {
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

template <class T, class S> inline void sample(const BasicImage<S>& im, double x, double y, T& result)
{
  int lx = (int)x;
  int ly = (int)y;
  x -= lx;
  y -= ly;
  result = static_cast<T>((1-y)*((1-x)*im[ly][lx] + x*im[ly][lx+1]) + y * ((1-x)*im[ly+1][lx] + x*im[ly+1][lx+1]));
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
template <class T> void transform(const BasicImage<T>& in, BasicImage<T>& out, const TooN::Matrix<2>& M, const TooN::Vector<2>& inOrig, const TooN::Vector<2>& outOrig)
  {
    int i,j;
    int w = out.size().x, iw = in.size().x;
    int h = out.size().y, ih = in.size().y;
    TooN::Vector<2> upperLeft = M * -outOrig + inOrig;
    TooN::Vector<2> dcol = M.T()[0];
    TooN::Vector<2> drow = M.T()[1];
    TooN::Vector<2> p, row = upperLeft;
    for (i=0;i<h;i++) {
      p = row;
      for (j=0;j<w;j++) {
	if (p[0] < 0 || p[0]>= iw-1 || p[1] < 0 || p[1] >= ih-1)
	  zeroPixel(out[i][j]);
	else
	  sample(in,p[0],p[1],out[i][j]);
	p += dcol;
      }
      row += drow;
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


}; // namespace CVD

#endif // CVD_VISION_H_
