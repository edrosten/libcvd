#include <cassert>
#include <cvd/vision.h>
#include <cvd/config.h>

using namespace std;

namespace CVD {

namespace Internal {

void convolveSeparableGray(unsigned char* I, unsigned int width, unsigned int height, const int kernel[], unsigned int size, int divisor)
{
    unsigned char buffer[width>height ? width : height];
    unsigned char* p = I;
    unsigned int i,j,m;
    if (size%2 == 0) {
        printf("In convolveSeparable, size must be odd.\n");
        return;
    }
    for (i=height; i>0; i--) {
        for (j=0;j<width-size+1;j++) {
            int sum = 0;
            for (m=0; m<size; m++)
                sum += p[j+m] * kernel[m];
            buffer[j] = (unsigned char)(sum/divisor);
        }
        memcpy(p+size/2, buffer, width-size+1);
        p += width;
    }
    for (j=0;j<width-size+1;j++) {
        p = I+j+(size/2)*width;
        for (i=0; i<height;i++)
            buffer[i] = I[i*width+j];
        for (i=0;i<height-size+1;i++) {
            int sum = 0;
            for (m=0; m<size; m++)
                sum += buffer[i+m] * kernel[m];
            *p = (unsigned char)(sum/divisor);
            p += width;
        }
    }
}

// TODO:: not used at all ?
void convolveSeparableRGB(unsigned char* I, unsigned int width, unsigned int height, const int kernel[], unsigned int size, int divisor)
{
    unsigned char buffer[(width>height ? width : height)*3];
    unsigned char* p = I;
    unsigned int i,j,m;
    if (size%2 == 0) {
        printf("In convolveSeparable, size must be odd.\n");
        return;
    }
    for (i=height; i>0; i--) {
        for (j=0;j<width-size+1;j++) {
            int sum = 0;
            for (m=0; m<size; m++)
                sum += p[j+m] * kernel[m];
            buffer[j] = (unsigned char)(sum/divisor);
        }
        memcpy(p+size/2, buffer, width-size+1);
        p += width;
    }
    for (j=0;j<width-size+1;j++) {
        p = I+j+(size/2)*width;
        for (i=0; i<height;i++)
            buffer[i] = I[i*width+j];
        for (i=0;i<height-size+1;i++) {
            int sum = 0;
            for (m=0; m<size; m++)
                sum += buffer[i+m] * kernel[m];
            *p = (unsigned char)(sum/divisor);
            p += width;
        }
    }
}

};

void convolveGaussian5_1(Image<byte>& I)
{
    int w = I.size().x;
    int h = I.size().y;
    int i,j;
    for (j=0;j<w;j++) {
        byte* src = I.data()+j;
        byte* end = src + w*(h-4);
        while (src != end) {
            int sum= (3571*(src[0]+src[4*w])
                    + 16004*(src[w]+src[3*w])
                    + 26386*src[2*w]);
            *(src) = sum >> 16;
            src += w;
        }
    }
    for (i=h-5;i>=0;i--) {
        byte* src = I.data()+i*w;
        byte* end = src + w-4;
        while (src != end) {
            int sum= (3571*(src[0]+src[4])
                    + 16004*(src[1]+src[3])
                    + 26386*src[2]);
            *(src+2*w+2) = sum >> 16;
            ++src;
        }
    }
}

#ifdef CVD_HAVE_MMXEXT
void halfSample(const Image<byte>& in, Image<byte>& out)
{
    assert(out.size().x == in.size().x/2 && out.size().y == in.size().y/2);
    Internal::halfsample(in.data(), out.data(), in.size().x, in.size().y);
}
#endif

#ifdef CVD_HAVE_SSE
void gradient(const Image<byte>& im, Image<float[2]>& out)
{
    assert(im.size() == out.size());
    Internal::byte_to_float_gradient(im.data(), out.data(), im.size().x, im.size().y);
    zeroBorders(out);
}
#endif

#ifdef CVD_HAVE_SSE2
void gradient(const Image<byte>& im, Image<double[2]>& out)
{
    assert(im.size() == out.size());
    Internal::byte_to_double_gradient(im.data(), out.data(), im.size().x, im.size().y);
    zeroBorders(out);
}
#endif

};
