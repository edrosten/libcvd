#include <cvd/convolution.h>

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_XMMINTRIN)
#include <xmmintrin.h>
#endif

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

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_XMMINTRIN)

inline void convolveMiddle5(const float* in, double factor, const double kernel[], int count, float* out)
{
    int i;
    const __m128 ffff = _mm_set1_ps(factor);
    const __m128 k1 = _mm_set1_ps(kernel[0]);
    const __m128 k2 = _mm_set1_ps(kernel[1]);
    for (i=0; i+3<count; i+=4, in += 4, out += 4) {
	__m128 sum = (_mm_mul_ps(_mm_loadu_ps(in), ffff) + 
		      _mm_mul_ps(k1, (_mm_loadu_ps(in-1) + _mm_loadu_ps(in+1))) + 
		      _mm_mul_ps(k2, (_mm_loadu_ps(in-2) + _mm_loadu_ps(in+2))));
	_mm_storeu_ps(out, sum);
    }

    for (; i<count; ++i, ++in, ++out) {
	double sum = in[0] * factor + kernel[0] * (in[-1] + in[1]) + kernel[1] * (in[-2] * in[2]);
	*out = sum;
    }
}

inline void convolveMiddle(const float* in, double factor, const vector<double>& kernel, int count, float* out)
{
    const int ksize = kernel.size();
    if (ksize == 2) {
	convolveMiddle5(in, factor, &kernel[0], count, out);
	return;
    }
    int i;
    const __m128 ffff = _mm_set1_ps(factor);
    for (i=0; i+3<count; i+=4, in += 4, out += 4) {
	__m128 sum = _mm_mul_ps(_mm_loadu_ps(in), ffff);
	for (int k=1; k<=ksize; ++k)
	    sum += _mm_set1_ps(kernel[k-1]) * (_mm_loadu_ps(in-k) + _mm_loadu_ps(in+k));
	_mm_storeu_ps(out, sum);
    }

    for (; i<count; ++i, ++in, ++out) {
	double sum = in[0] * factor;
	for (int k=1; k<=ksize; ++k)
	    sum += kernel[k-1] * (in[-k] + in[k]);
	*out = sum;
    }
}

inline void convolveVertical5(const vector<float*> row, double factor, double kernel[], int count, float* out)
{
    const int ksize = 2;
    int i;
    for (i=0; i<count && !is_aligned<16>(row[0] + i); ++i, ++out) {
	double sum = row[ksize][i] * factor + (row[1][i] + row[3][i]) * kernel[0] + (row[0][i] + row[4][i]) * kernel[1];
	*out = sum;
    }

    const __m128 ffff = _mm_set1_ps(factor);
    const __m128 k1 = _mm_set1_ps(kernel[0]);
    const __m128 k2 = _mm_set1_ps(kernel[1]);

    for (; i+3<count; i+=4) {
	__m128 sum = (_mm_load_ps(row[ksize] + i) * ffff
		      + (_mm_load_ps(row[ksize-1]+i) + _mm_load_ps(row[ksize+1]+i)) * k1
		      + (_mm_load_ps(row[ksize-2]+i) + _mm_load_ps(row[ksize+2]+i)) * k2);
	_mm_store_ps(out + i, sum);
    }    
    for (; i<count; ++i, ++out) {
	double sum = row[ksize][i] * factor + (row[1][i] + row[3][i]) * kernel[0] + (row[0][i] + row[4][i]) * kernel[1];
	*out = sum;
    }
}

inline void convolveVertical(const vector<float*> row, double factor, vector<double>& kernel, int count, float* out)
{
    const int ksize = kernel.size();
    if (ksize == 2) {
	convolveVertical5(row, factor, &kernel[0], count, out);
	return;
    }
	
    int i;
    for (i=0; i<count && !is_aligned<16>(row[0] + i); ++i, ++out) {
	double sum = row[ksize][i] * factor;
	for (int k=1; k<=ksize; ++k)
	    sum += kernel[k-1] * (row[ksize-k][i] + row[ksize+k][i]);
	*out = sum;
    }
    const __m128 ffff = _mm_set1_ps(factor);
    for (; i+3<count; i+=4) {
	__m128 sum = _mm_mul_ps(_mm_load_ps(row[ksize] + i), ffff);
	for (int k=1; k<=ksize; ++k)
	    sum += _mm_set1_ps(kernel[k-1]) * (_mm_load_ps(row[ksize-k]+i) + _mm_load_ps(row[ksize+k]+i));
	_mm_store_ps(out + i, sum);
    }    
    for (; i<count; ++i, ++out) {
	double sum = row[ksize][i] * factor;
	for (int k=1; k<=ksize; ++k)
	    sum += kernel[k-1] * (row[ksize-k][i] + row[ksize+k][i]);
	*out = sum;
    }
}

void convolveGaussian(const BasicImage<float>& I, BasicImage<float>& out, double sigma, double sigmas)
{
    assert(out.size() == I.size());
    int ksize = (int)(sigmas*sigma + 0.5);
    vector<double> kernel(ksize);
    double ksum = 1.0;
    for (int i=1; i<=ksize; i++)
	ksum += 2*(kernel[i-1] = exp(-i*i/(2*sigma*sigma)));
    double factor = 1.0/ksum;
    for (int i=0; i<ksize; i++)
	kernel[i] *= factor;
    const int w = I.size().x;
    const int h = I.size().y;
    const int swin = 2*ksize;

    AlignedMem<float,16> buffer(w*(swin+1));
    vector<float*> rows(swin+1);

    for (int k=0;k<swin+1;k++)
	rows[k] = buffer.data() + k*w;

    float* output = out.data();
    for (int i=0; i<h; i++) {
	float* next_row = rows[swin];
	const float* input = I[i];
	// beginning of row
	for (int j=0; j<ksize; j++) {
	    double hsum = input[j] * factor;
	    for (int k=0; k<ksize; k++)
		hsum += (input[abs(j-k-1)] + input[j+k+1]) * kernel[k];
	    next_row[j] = hsum;
	}
	// middle of row
	input += ksize;
	convolveMiddle(input, factor, kernel, w-swin, next_row+ksize);
	input += w-swin;
	// end of row
	for (int j=w-ksize; j<w; j++, input++) {
	    double hsum = *input * factor;
	    const int room = w-j;
	    for (int k=0; k<ksize; k++) {
		hsum += (input[-k-1] + (k+1 >= room ? input[2*room-k-2] : input[k+1])) * kernel[k];
	    }
	    next_row[j] = hsum;
	}
	// vertical
	if (i >= swin) {
	    convolveVertical(rows, factor, kernel, w, output);
	    output += w;
	    if (i == h-1) {
		for (int r=0; r<ksize; r++, output += w) {
		    vector<float*> rrows(rows.size());
		    rrows[ksize] = rows[ksize+r+1];
		    for (int k=0; k<ksize; ++k) {
			rrows[ksize-k-1] = rows[ksize+r-k];
			int hi = ksize+r+k+2;
			rrows[ksize+k+1] = rows[hi > swin ? 2*swin - hi : hi];
		    }
		    convolveVertical(rrows, factor, kernel, w, output);
		}
	    }
	} else if (i == swin-1) {
	    for (int r=0; r<ksize; r++, output += w) {
		vector<float*> rrows(rows.size());
		rrows[ksize] = rows[r+1];
		for (int k=0; k<ksize; ++k) {
		    rrows[ksize-k-1] = rows[abs(r-k-1)+1];
		    rrows[ksize+k+1] = rows[r+k+2];
		}
		convolveVertical(rrows, factor, kernel, w, output);
	    }
	}
    
	float* tmp = rows[0];
	for (int r=0;r<swin; r++)
	    rows[r] = rows[r+1];
	rows[swin] = tmp;
    }
}
#endif


};

