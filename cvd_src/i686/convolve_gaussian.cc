#include <cvd/convolution.h>
#include <xmmintrin.h>

using namespace std;

namespace CVD {

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

};

