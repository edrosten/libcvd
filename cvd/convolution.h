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

#ifndef CVD_CONVOLUTION_H_
#define CVD_CONVOLUTION_H_

#include <vector>

#include <cvd/config.h>
#include <cvd/exceptions.h>
#include <cvd/image.h>
#include <cvd/internal/pixel_operations.h>

namespace CVD {

#undef CVD_HAVE_SSE
// internal functions used by CVD convolution algorithm implementations
namespace Internal {
extern "C"{

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
void convolve_float4(float (*I)[4], int w, int h, float* kernel, int k);
void convolve_float(float *I, int w, int h, float* kernel, int k);
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

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
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

template <class T, int A, int B, int C> void convolveSymmetric(Image<T>& I)
  {
    typedef typename Pixel::traits<T>::wider_type wider;
    static const wider S = (A+B+C+B+A);
    int width = I.size().x;
    int height = I.size().y;
    T* p = I.data();
    int i,j;
    for (i=0; i<height; i++) {
      wider a = p[0];
      wider b = p[1];
      wider c = p[2];
      wider d = p[3];
      p[0] = (T)(((c+c)*A+(b+b)*B + a*C) /S);
      p[1] = (T)(((b+d)*A+(a+c)*B + b*C) /S);
      for (j=0;j<width-4;j++,p++) {
        wider e = p[4];
        p[2] = (T)(((a+e)*A + (b+d)*B + c*C)/S);
        a = b; b = c; c = d; d = e;
      }
      p[2] = (T)(((a+c)*A + (b+d)*B + c*C) /S);
      p[3] = (T)(((b+b)*A + (c+c)*B + d*C) /S);
      p += 4;
    }
    for (j=0;j<width;j++) {
      p = I.data()+j;
      wider a = p[0];
      wider b = p[width];
      p[0] = (T)(((p[2*width]+p[2*width])*A+(b+b)*B + a*C) /S);
      p[width] = (T)(((b+p[width*3])*A+(a+p[2*width])*B + b*C) /S);
      for (i=0;i<height-4;i++) {
        wider c = p[2*width];
        p[2*width] = (T)(((a+p[4*width])*A + (b+p[3*width])*B + c*C)/S);
        a=b; b=c;
        p += width;
      }
      wider c = p[2*width];
      p[2*width] = (T)(((a+c)*A + (b+p[width*3])*B + c*C) /S);
      p[3*width] = (T)(((b+b)*A + (c+c)*B + p[width*3]*C) /S);
    }
  }

  template <class T, int A, int B, int C, int D> void convolveSymmetric(Image<T>& I)
  {
    typedef typename Pixel::traits<T>::wider_type wider;
    static const wider S = (A+B+C+D+C+B+A);
    int width = I.size().x;
    int height = I.size().y;
    T* p = I.data();
    int i,j;
    for (i=0; i<height; i++) {
      wider a = p[0];
      wider b = p[1];
      wider c = p[2];
      wider d = p[3];
      p[0] = (T)(((d+d)*A + (c+c)*B + (b+b)*C + a*D)/S);
      p[1] = (T)(((c+p[4])*A + (b+d)*B + (a+c)*C + b*D)/S);
      p[2] = (T)(((b+p[5])*A + (a+p[4])*B + (b+d)*C + c*D)/S);
      for (j=0;j<width-6;j++,p++) {
        d = p[3];
        p[3] = (T)(((a+p[6])*A + (b+p[5])*B + (c+p[4])*C + d*D)/S);
        a=b; b=c; c=d;
      }
      d = p[3];
      wider e = p[4];
      p[3] = (T)(((a+e)*A + (b+p[5])*B + (c+e)*C + d*D)/S);
      p[4] = (T)(((b+d)*A + (c+e)*B + (d+p[5])*C + e*D)/S);
      p[5] = (T)(((c+c)*A + (d+d)*B + (e+e)*C + p[5]*D)/S);
      p += 6;
    }
    for (j=0;j<width;j++) {
      p = I.data()+j;
      wider a = p[0];
      wider b = p[width];
      wider c = p[2*width];
      wider d = p[3*width];
      p[0] = (T)(((d+d)*A + (c+c)*B + (b+b)*C + a*D)/S);
      p[width] = (T)(((c+p[4*width])*A + (b+d)*B + (a+c)*C + b*D)/S);
      p[2*width] = (T)(((b+p[5*width])*A + (a+p[4*width])*B + (b+d)*C + c*D)/S);
      for (i=0;i<height-6;i++) {
        d = p[3*width];
        p[3*width] = (T)(((a+p[width*6])*A + (b+p[width*5])*B + (c+p[width*4])*C+d*D)/S);
        a=b; b=c; c=d;
        p += width;
      }
      d = p[3*width];
      wider e = p[4*width];
      p[3*width] = (T)(((a+e)*A + (b+p[5*width])*B + (c+e)*C + d*D)/S);
      p[4*width] = (T)(((b+d)*A + (c+e)*B + (d+p[5*width])*C + e*D)/S);
      p[5*width] = (T)(((c+c)*A + (d+d)*B + (e+e)*C + p[5*width]*D)/S);
    }
  }

    template <class T, class K> void convolveSeparableSymmetric(Image<T>& I, const std::vector<K>& kernel, K divisor)
  {
    typedef typename Pixel::traits<T>::wider_type sum_type;
    int w = I.size().x;
    int h = I.size().y;
    int r = (int)kernel.size()/2;
    int i,j;
    int m;
    double factor = 1.0/divisor;
    for (j=0;j<w;j++) {
      T* src = I.data()+j;
      for (i=0; i<h-2*r; i++,src+=w) {
        sum_type sum, v;
        Pixel::operations<sum_type>::assign(sum, src[r*w]);
        Pixel::operations<sum_type>::multiply(sum, kernel[r]);
        for (m=0; m<r; m++) {
          Pixel::operations<sum_type>::assign(v, src[m*w]);
          Pixel::operations<sum_type>::add(v, src[(2*r-m)*w]);
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
        Pixel::operations<sum_type>::assign(sum, src[r]);
        Pixel::operations<sum_type>::multiply(sum, kernel[r]);
        for (m=0; m<r; m++) {
          Pixel::operations<sum_type>::assign(v, src[m]);
          Pixel::operations<sum_type>::add(v, src[2*r-m]);
          Pixel::operations<sum_type>::multiply(v, kernel[m]);
          Pixel::operations<sum_type>::add(sum,v);
        }
        Pixel::operations<sum_type>::multiply(sum, factor);
        Pixel::operations<T>::assign(*(src+r*w+r), sum);
      }
    }
  }
  
} // namespace CVD

#endif
