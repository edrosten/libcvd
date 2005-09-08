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

// internal functions used by CVD vision algorithm implementations
namespace Internal {
extern "C"{
#ifdef CVD_HAVE_MMXEXT
void halfsample(const unsigned char* in, unsigned char* out, int width, int height);
#endif

#ifdef CVD_HAVE_SSE
void byte_to_float_gradient(const unsigned char* gray, const float (*grad)[2], int width, int height);
void convolve_float4(float (*I)[4], int w, int h, float* kernel, int k);
void convolve_float(float *I, int w, int h, float* kernel, int k);
//void box_convolve_float4(float (*I)[4], int w, int h, int hwin, float factor);
// void float_second_moment(const float (*grad)[2], int size, float* Gx, float* Gy, float* Gxy);
#endif

#ifdef CVD_HAVE_SSE2
void byte_to_double_gradient(const unsigned char* gray, const double (*grad)[2], int width, int height);
#endif
};
void convolveSeparableGray(unsigned char* I, unsigned int width, unsigned int height, const int kernel[], unsigned int size, int divisor);
};

/// creates a Gaussian kernel with given maximum value and standard deviation.
/// All elements of the passed vector are filled up, therefore the vector
/// defines the size of the computed kernel. The normalizing value is returned.
/// @param k vector of T's holds the kernel values
/// @param maxval the maximum value to be used
/// @param stddev standard deviation of the kernel
/// @return the sum of the kernel elements for normalization
/// @ingroup gVision
template <class T>
T gaussianKernel(std::vector<T>& k, T maxval, double stddev)
{
    double sum = 0;
    unsigned int i, argmax=0;
    std::vector<double> kernel(k.size());
    for (i=0;i<k.size();i++) {
        double x = i +0.5 - k.size()/2.0;
        sum += kernel[i] = exp(-x*x/(2*stddev*stddev));
        if (kernel[i] > kernel[argmax])
        argmax = i;
    }
    T finalSum = 0;
    for (i=0;i<k.size();i++)
    finalSum += k[i] = (T)(kernel[i]*maxval/kernel[argmax]);
    return finalSum;
}

/// scales a GaussianKernel to a different maximum value. The new kernel is
/// returned in scaled. The new normalizing value is returned.
/// @param k input kernel
/// @param scaled output vector to hold the resulting kernel
/// @param maxval the new maximum value
/// @return sum of the new kernel elements for normalization
/// @ingroup gVision
template <class S, class T>
T scaleKernel(const std::vector<S>& k, std::vector<T>& scaled, T maxval)
{
    unsigned int i,argmax=0;
    for (i=1;i<k.size();i++)
        if (k[i]>k[argmax])
            argmax = i;
    scaled.resize(k.size());
    T sum = 0;
    for (i=0;i<k.size();i++)
        sum += (scaled[i] = (T)((k[i]*maxval)/k[argmax]));
    return sum;
}

/// convolves an image with a separable kernel described by a vector and a
/// normalization factor (such as returned by gaussianKernel). The convolution
/// is implemented in place and will change the argument image. On platforms
/// supporting the extended MMX instruction set, optimized implementations
/// are used for some types.
/// @param I image to be convolved
/// @param kernel a vector containing the kernel values
/// @param divisor the sum of the kernel values for normalization
/// @ingroup gVision
template <class T, class K> void convolveSeparable(BasicImage<T>& I, const std::vector<K>& kernel, K divisor)
{
    typedef typename Pixel::traits<T>::wider_type sum_type;
    int w = I.size().x;
    int h = I.size().y;
    int r = (int)kernel.size()/2;
    int i,j;
    unsigned int m;
    double factor = 1.0/divisor;
    for (j=0;j<w;j++) {
        T* src = I.data()+j;
        for (i=0; i<h-2*r; i++,src+=w) {
            sum_type sum, v;
            Pixel::operations<sum_type>::zero(sum);
            for (m=0; m<kernel.size(); m++) {
            Pixel::operations<sum_type>::assign(v, src[m*w]);
            Pixel::operations<sum_type>::multiply(v, kernel[m]);
            Pixel::operations<sum_type>::add(sum,v);
            }
            Pixel::operations<sum_type>::multiply(sum, factor);
            Pixel::operations<T>::assign(*(src), sum);
        }
    }
    for (i=h-2*r-1;i>=0;i--) {
        T* src = I.data()+i*w;
        for (j=0;j<w-2*r;j++, src++) {
            sum_type sum, v;
            Pixel::operations<sum_type>::zero(sum);
            for (m=0; m<kernel.size(); m++) {
            Pixel::operations<sum_type>::assign(v, src[m]);
            Pixel::operations<sum_type>::multiply(v, kernel[m]);
            Pixel::operations<sum_type>::add(sum,v);
            }
            Pixel::operations<sum_type>::multiply(sum, factor);
            Pixel::operations<T>::assign(*(src+r*w+r), sum);
        }
    }
}

static inline void convolveSeparable(BasicImage<byte>& I, const std::vector<int>& kernel, int divisor)
{
    Internal::convolveSeparableGray(I.data(), I.size().x, I.size().y, &kernel[0], kernel.size(), divisor);
}

#ifdef CVD_HAVE_MMXEXT
static inline void convolveSeparable(BasicImage<float[4]>& I, const std::vector<float>& kernel, float divisor)
{
    std::vector<float> sk = kernel;
    for (unsigned int i=0; i<sk.size(); i++)
    sk[i] /= divisor;
    Internal::convolve_float4(I.data(), I.size().x, I.size().y, &sk[0], (int)sk.size());
}

static inline void convolveSeparable(BasicImage<float>& I, const std::vector<float>& kernel, float divisor)
{
    std::vector<float> sk = kernel;
    for (unsigned int i=0; i<sk.size(); i++)
    sk[i] /= divisor;
    Internal::convolve_float(I.data(), I.size().x, I.size().y, &sk[0], (int)sk.size());
}
#endif

template <class T>
void convolveGaussian5_1(BasicImage<T>& I)
{
    int w = I.size().x;
    int h = I.size().y;
    int i,j;
    for (j=0;j<w;j++) {
        T* src = I.data()+j;
        T* end = src + w*(h-4);
        while (src != end) {
            T sum= (T)(0.0544887*(src[0]+src[4*w])
                    + 0.2442010*(src[w]+src[3*w])
                    + 0.4026200*src[2*w]);
            *(src) = sum;
            src += w;
        }
    }
    for (i=h-5;i>=0;i--) {
        T* src = I.data()+i*w;
        T* end = src + w-4;
        while (src != end) {
            T sum= (T)(0.0544887*(src[0]+src[4])
                    + 0.2442010*(src[1]+src[3])
                    + 0.4026200*src[2]);
            *(src+2*w+2)=sum;
            ++src;
        }
    }
}

void convolveGaussian5_1(BasicImage<byte>& I);


// TODO: this was using aligned memory, check if this is necessary...
/// convolves an image with a box of given size.
/// @param I input image, modified in place
/// @param hwin window size, this is half of the box size
/// @ingroup gVision
template <class T>
void convolveWithBox(BasicImage<T>& I, int hwin)
{
    typedef typename Pixel::traits<T>::wider_type sum_type;
    int w = I.size().x;
    int h = I.size().y;
    int i,j;
    int win = 2*hwin+1;
    //sum_type* sums = aligned_mem<sum_type,16>::alloc(w);
    sum_type sums[w];
    memset(sums, 0, sizeof(sum_type)*w);

    char buffer[64];
    int val = (int)buffer;
    T& sum = *(T*)(buffer + (16 - (val%16)));
    T& tmp = *(&sum + 1);

    T* row = I.data();
    T* old = I.data();
    for (i=0; i<hwin*2; i++) {
        for (j=0;j<w; j++)
            Pixel::operations<sum_type>::add(sums[j],*(row++));
    }
    for (; i<h; i++) {
        T* s = sums;
        for (j=0;j<w;j++, old++, row++, s++) {
            Pixel::operations<sum_type>::add(*s, *row);
            Pixel::operations<sum_type>::assign(tmp, *old);
            Pixel::operations<T>::assign(*old, *s);
            Pixel::operations<sum_type>::subtract(*s, tmp);
        }
    }
    //aligned_mem<float[4],16>::release(sums);
    int offset = hwin+w*hwin;
    for (i=h-win; i>=0; i--) {
        row = I.data()+i*w;
        old = row;
        Pixel::operations<sum_type>::zero(sum);
        for (j=0;j<hwin*2;j++, row++)
            Pixel::operations<sum_type>::add(sum, *row);
        for (;j<w;j++, row++, old++) {
            Pixel::operations<sum_type>::add(sum, *row);
            Pixel::operations<sum_type>::assign(tmp, sum);
            Pixel::operations<sum_type>::subtract(sum, *old);
            Pixel::operations<sum_type>::divide(tmp, win*win);
            Pixel::operations<T>::assign(*(old+offset), tmp);
        }
    }
}

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
    int row = 0;
    T* p = out.data();
    sum_type sum;
    while (row < in.size().y-1) {
        for (int j=0; j<in.size().x/2; j++) {
            Pixel::operations<sum_type>::assign(sum, top[j*2]);
            Pixel::operations<sum_type>::add(sum, top[j*2+1]);
            Pixel::operations<sum_type>::add(sum, bottom[j*2]);
            Pixel::operations<sum_type>::add(sum, bottom[j*2+1]);
            Pixel::operations<sum_type>::divide(sum, 4);
            Pixel::operations<T>::assign(p[j], sum);
        }
        top += in.size().x*2;
        bottom += in.size().x*2;
        p += out.size().x;
        row += 2;
    }
}

#ifdef CVD_HAVE_MMXEXT
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
    T* p = im.data();
    const T* end = im.data()+im.totalsize();
    while (p != end) {
        if (*p < minimum)
            *p = T();
        else
            *p = hi;
        ++p;
    }
}

/// sets the border pixel lines of an image to zero
/// @param I input image, changed in place
/// @ingroup gVision
template <class T> void zeroBorders(BasicImage<T>& I)
{
    int w = I.size().x;
    int h = I.size().y;
    memset(I.data(), 0, sizeof(T)*w);
    for (int i=1;i<h-1; i++) {
        Pixel::operations<T>::zero(I.data()[i*w]);
        Pixel::operations<T>::zero(I.data()[i*w+w-1]);
    }
    memset(I.data()+w*(h-1), 0, sizeof(T)*w);
}

/// computes the gradient image from an image. The gradient image contains two components per pixel holding
/// the x and y components of the gradient.
/// @param im input image
/// @param out output image, must have the same dimensions as input image
/// @throw IncompatibleImageSizes if out does not have same dimensions as im
/// @ingroup gVision
template <class S, class T>
void gradient(const BasicImage<S>& im, BasicImage<T>& out)
{
    typedef typename Pixel::Component<T>::type TComp;
    typedef typename Pixel::Component<S>::type SComp;
    typedef typename Pixel::traits<SComp>::wider_type diff_type;

    if( im.size() != out.size())
        throw Exceptions::Vision::IncompatibleImageSizes("gradient");
    int w = im.size().x;
    T* dp = out.data()+w+1;
    const S* sp = im.data()+w+1;
    const S* end = im.data()+im.totalsize()-w-1;
    while (sp != end) {
      (*dp)[0] = Pixel::scalar_convert<TComp,diff_type>::from(Pixel::difference<S>::avg(sp[1], sp[-1]));
      (*dp)[1] = Pixel::scalar_convert<TComp,diff_type>::from(Pixel::difference<S>::avg(sp[w], sp[-w]));
      ++sp;
      ++dp;
    }
    zeroBorders(out);
}

#ifdef CVD_HAVE_SSE
void gradient(const BasicImage<byte>& im, BasicImage<float[2]>& out);
#endif
#ifdef CVD_HAVE_SSE2
void gradient(const BasicImage<byte>& im, BasicImage<double[2]>& out);
#endif

/// computes mean and stddev of intensities in an image. These are computed for each component of the
/// pixel type, therefore the output are two pixels with mean and stddev for each component.
/// @param im input image
/// @param mean pixel element containing the mean of intensities in the image for each component
/// @param stddev pixel element containing the standard deviation for each component
/// @ingroup gVision
template <class T>
void stats(const BasicImage<T>& im, T& mean, T& stddev)
{
    const unsigned int c = Pixel::Component<T>::count;
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

/// a functor computing gray values from pixels with constant value. For use with apply to
/// operate on all pixels in an image
/// @ingroup gVision
template <class S, class T, class PixelFunction = Pixel::BasicConversion<> >
struct Gray {
    typedef typename Pixel::Component<S>::type SComp;
    typedef typename Pixel::Component<T>::type TComp;
    PixelFunction c;
    inline void operator()(const S& s, T& t) const {
        c.convert_pixel(s, Pixel::Component<T>::get(t,0));
        for (unsigned int i=1; i<Pixel::Component<T>::count; i++)
            Pixel::Component<T>::get(t,i) = Pixel::Component<T>::get(t,0);
    }
};

/// a functor multiplying pixels with constant value. For use with apply to
/// operate on all pixels in an image
/// @ingroup gVision
template <class S, class T = S>
struct multiplyBy
{
    typename Pixel::Component<T>::type op;
    multiplyBy( const typename Pixel::Component<T>::type & op_ ) : op(op_) {};
    inline void operator()(const S& s, T& t) const
    {
        Pixel::operations<T>::assign(t,s);
        Pixel::operations<T>::multiply(t, op);
    };
};

/// applies a pixel operation to each pixel in an image and returns the result in an output image
/// @param in the input image
/// @param out the output image, can be the same as the input image
/// @param op a functor implementation the operation. The functor must implement the operator()(const S& , const T&) or be a function with that signature.
/// @throw IncompatibleImageSizes if the images are not of the same sizes
/// @ingroup gVision
template <class S, class T, class Op>
void apply(const BasicImage<S>& in, BasicImage<T>& out, const Op& op)
{
    if(out.size() != in.size())
        throw Exceptions::Vision::IncompatibleImageSizes("apply");

    const S* pi = in.data();
    const S* end = pi+in.totalsize();
    T* po = out.data();
    while (pi != end)
        op(*pi++, *po++);
}

/// searches through the image and returns pixel locations that match a given predicate. this
/// version returns a new vector containing the image references.
/// @param in the image to search through
/// @param op the predicate to test with. It must implement bool operator()( const ImageRef & ) or be a function with that signature
/// @return vector containing ImageRef with pixel matching the predicate
template <class T, class Op>
std::vector<ImageRef> find( const BasicImage<T> & in, const Op & op)
{
    std::vector<ImageRef> list;
    find(in, op, list);
    return list;
}

/// searches through the image and returns pixel locations that match a given predicate. this
/// version will append found image references to a vector passed in.
/// @param in the image to search through
/// @param op the predicate to test with. It must implement bool operator()( const ImageRef & ) or be a function with that signature
/// @param list vector containing the image references
template <class T, class Op>
std::vector<ImageRef> & find( const BasicImage<T> & in, const Op & op, std::vector<ImageRef> & list)
{
    ImageRef begin(0,0);
    const ImageRef end = in.size();
    do {
        if(op(in[begin]))
            list.push_back(begin);
    } while(begin.next(end));
    return list;
}

/*
// TODO: do we need these at all ?

template<template <class P> class PixelFunction=Pixel::pixel_norm>
struct desaturate {
    template <class S, class T> void operator()(const BasicImage<S>& I, BasicImage<T>& D) {
        apply(I,D,Gray<S,T,PixelFunction>());
    }
    template <class T> void operator()(BasicImage<T>& I){
        apply(I,I,Gray<T,T,PixelFunction>());
    }
};
*/

  /*
    template <class T, int N>
    void subtract(BasicImage<T[N]>& I, const BasicImage<T[N]>& operand)
    {
        typedef typename traits<T>::wider_type wider;
        assert(operand.size() == I.size());
        T *p = (T*)I.data(), *end = (T*)I.data() + I.totalsize()*N;
        T *q = (T*)operand.data();
        while (p!=end)
        {
            wider diff = *p;
            diff -= *q++;
            *p++ = (T)CLAMP(diff,0,(wider)traits<T>::max_intensity);
        }
    }
*/

}; // namespace CVD

#endif // CVD_VISION_H_
