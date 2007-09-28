#include <cvd/vision.h>
#include <cvd/config.h>
// internal functions used by CVD vision algorithm implementations
#include <cvd/internal/assembly.h>

#if defined( __GNUC__) && defined(CVD_HAVE_SSE2)
#include <emmintrin.h>
#endif

using namespace std;

namespace CVD {

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
    
#if defined( __GNUC__) && defined(CVD_HAVE_SSE2)

    // I have to use this because gcc 3.3 has an internal bug with _mm_slli_epi16
#define shift_left_7(thing) asm ( "psllw $0x7, %0  \n\t" : : "x"(thing) : "%0" );
    
    void gradient(const byte* in, short (*out)[2], int w, int h)
    {
	const byte zeros[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	const byte* curr = in + w;
	out += w;
	int total = (w*(h-2)) >> 4;
	__m128i zero = _mm_loadu_si128((const __m128i*)zeros);
	while (total--) {
	    __m128i up = _mm_load_si128((const __m128i*)(curr-w));
	    __m128i down = _mm_load_si128((const __m128i*)(curr+w));	    
	    __m128i hor_left, hor_right;
	    {
		__m128i hor = _mm_load_si128((const __m128i*)curr);
		hor_left = _mm_slli_si128(hor, 1);
		hor_right = _mm_srli_si128(hor, 1);
	    }
	    {
		__m128i left = _mm_unpacklo_epi8(hor_right, zero);
		__m128i right = _mm_unpacklo_epi8(hor_left, zero);
		__m128i hdiff = _mm_insert_epi16(_mm_sub_epi16(right,left), short(curr[1]) - short(curr[-1]), 0);
		__m128i vdiff = _mm_sub_epi16(_mm_unpacklo_epi8(down, zero), _mm_unpacklo_epi8(up, zero));
		{
		    __m128i part1 = _mm_unpacklo_epi16(hdiff, vdiff);
		    shift_left_7(part1);
		    _mm_stream_si128((__m128i*)out, part1);
		}
		{
		    __m128i part2 = _mm_unpackhi_epi16(hdiff, vdiff);
		    shift_left_7(part2);
		    _mm_stream_si128((__m128i*)(out+4), part2);
		}
	    }
	    {
		__m128i left = _mm_unpackhi_epi8(hor_right, zero);
		__m128i right = _mm_unpackhi_epi8(hor_left, zero);
		__m128i hdiff = _mm_insert_epi16(_mm_sub_epi16(right,left), short(curr[16]) - short(curr[14]), 7);
		__m128i vdiff = _mm_sub_epi16(_mm_unpackhi_epi8(down, zero),_mm_unpackhi_epi8(up, zero));
		{
		    __m128i part3 = _mm_unpacklo_epi16(hdiff, vdiff);
		    shift_left_7(part3);
		    _mm_stream_si128((__m128i*)(out+8), part3);
		}
		{
		    __m128i part4 = _mm_unpackhi_epi16(hdiff, vdiff);
		    shift_left_7(part4);
		    _mm_stream_si128((__m128i*)(out+12), part4);
		}
	    }
	    curr += 16;
	    out += 16;
	}
	_mm_empty();
    }

    void halfSampleSSE2(const byte* in, byte* out, int w, int h) 
    {
	const unsigned long long mask[2] = {0x00FF00FF00FF00FFull, 0x00FF00FF00FF00FFull};
	byte test[16];
	const byte* nextRow = in + w;
	__m128i m = _mm_loadu_si128((const __m128i*)mask);
	int sw = w >> 4;
	int sh = h >> 1;
	for (int i=0; i<sh; i++) {
	    for (int j=0; j<sw; j++) {
		__m128i here = _mm_load_si128((const __m128i*)in);
		__m128i next = _mm_load_si128((const __m128i*)nextRow);
		here = _mm_avg_epu8(here,next);
		next = _mm_and_si128(_mm_srli_si128(here,1), m);
		here = _mm_and_si128(here,m);
		here = _mm_avg_epu16(here, next);
		_mm_storel_epi64((__m128i*)out, _mm_packus_epi16(here,here));
		in += 16;
		nextRow += 16;
		out += 8;
	    }
	    in += w;
	    nextRow += w;
	}
    }
#endif

    void halfSample(const BasicImage<byte>& in, BasicImage<byte>& out)
    {   
	if( (in.size()/2) != out.size())
	    throw Exceptions::Vision::IncompatibleImageSizes("halfSample");

#if defined( __GNUC__) && defined(CVD_HAVE_SSE2)
	if (is_aligned<16>(in.data()) && is_aligned<16>(out.data()) && ((in.size().x % 16) == 0)) {
	    halfSampleSSE2(in.data(), out.data(), in.size().x, in.size().y);
	    return;
	}
#endif
    
	if (!is_aligned<8>(in.data()) || !is_aligned<8>(out.data()) || (in.size().x % 8 != 0))
	    halfSample<byte>(in, out);
	else
	    Internal::Assembly::halfsample(in.data(), out.data(), in.size().x, in.size().y);
    }
#endif


#if (CVD_HAVE_EMMINTRIN && CVD_HAVE_SSE2)
    
#define SORT2_SIMD_UB(a,b)			\
    {						\
	const __m128i ta = a;			\
	a = _mm_min_epu8(a,b);			\
	b = _mm_max_epu8(ta,b);			\
    }
    
#define SORT3_SIMD_UB(a,b,c)			\
    {						\
	SORT2_SIMD_UB(a,b);			\
	SORT2_SIMD_UB(b,c);			\
	SORT2_SIMD_UB(a,b);			\
    }
    
    namespace median {
	inline __m128i median3(__m128i a, __m128i b, __m128i c) {
	    SORT2_SIMD_UB(a,b);
	    return _mm_max_epu8(a,_mm_min_epu8(b,c));
	}
	
	__m128i median_3x3_simd(const byte* p, const int w)
	{
	    __m128i a = _mm_loadu_si128((const __m128i*)(p-w-1));
	    __m128i b = _mm_loadu_si128((const __m128i*)(p-w));
	    __m128i c = _mm_loadu_si128((const __m128i*)(p-w+1));
	    SORT3_SIMD_UB(a,b,c);
	    __m128i d = _mm_loadu_si128((const __m128i*)(p-1));
	    __m128i e = _mm_loadu_si128((const __m128i*)(p));
	    __m128i f = _mm_loadu_si128((const __m128i*)(p+1));
	    SORT3_SIMD_UB(d,e,f);
	    __m128i g = _mm_loadu_si128((const __m128i*)(p+w-1));
	    __m128i h = _mm_loadu_si128((const __m128i*)(p+w));
	    __m128i i = _mm_loadu_si128((const __m128i*)(p+w+1));
	    SORT3_SIMD_UB(g,h,i);
	    e = median3(b,e,h);
	    g = _mm_max_epu8(_mm_max_epu8(a,d), g);
	    c = _mm_min_epu8(c, _mm_min_epu8(f,i));
	    return median3(c,e,g);
	}
	
	void median_filter_3x3_simd(const byte* p, const int stride, const int n, byte* out)
	{
	    int j = 0;
	    for (; j+15<n; j+=16, p+=16, out += 16) {
		_mm_storeu_si128((__m128i*)out, median_3x3_simd(p,stride));
	    }
	    if (j < n) {
		const int left = n - j;
		if (n >= 16 && left > 8)
		    _mm_storeu_si128((__m128i*)(out+left-16), median_3x3_simd(p+left-16,stride));
		else
		    median_filter_3x3(p, stride, left, out);
	    }
	}
	
    }

    void median_filter_3x3(const SubImage<byte>& I, SubImage<byte> out)
    {
	assert(out.size() == I.size());
	const int s = I.row_stride();
	const int n = I.size().x - 2;
	for (int i=1; i+1<I.size().y; ++i)
	    median::median_filter_3x3_simd(I[i]+1, s, n, out[i]+1);
    }
#endif
    

};
