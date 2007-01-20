#include <cvd/fast_corner.h>

#include <vector>
#include <list>

using namespace CVD;
using namespace std;

#if (CVD_HAVE_EMMINTRIN && CVD_HAVE_SSE2)
#include <emmintrin.h>


namespace CVD
{



struct Less { template <class T1, class T2> static bool eval(const T1 a, const T2 b) { return a < b; }};
struct Greater { template <class T1, class T2> static bool eval(const T1 a, const T2 b) { return b < a; }};

template <class C> inline bool is_corner_12(const byte* p, int w, int t) {
    const int w3 = 3*w;
    if (!C::eval(p[-3],t)) {
        if (!C::eval(p[-w3],t) || !C::eval(p[1-w3],t) || !C::eval(p[2-2*w],t) || !C::eval(p[3-w],t) || !C::eval(p[3],t) || !C::eval(p[3+w],t) || !C::eval(p[2+2*w],t) || !C::eval(p[1+w3],t) || !C::eval(p[w3],t))
            return false;
        if (!C::eval(p[-1-w3],t))
            return (C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
        if (!C::eval(p[-2-2*w],t))
            return (C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t));
        if (!C::eval(p[-3-w],t))
            return (C::eval(p[-1+w3],t));
        return true;
    }
    if (!C::eval(p[-3-w],t)) {
        if (!C::eval(p[1-w3],t) || !C::eval(p[2-2*w],t) || !C::eval(p[3-w],t) || !C::eval(p[3],t) || !C::eval(p[3+w],t) || !C::eval(p[2+2*w],t) || !C::eval(p[1+w3],t) || !C::eval(p[w3],t) || !C::eval(p[-1+w3],t))
            return false;
        if (!C::eval(p[-w3],t)  || !C::eval(p[-1-w3],t))
            return (C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
        if (!C::eval(p[-2-2*w],t))
            return (C::eval(p[-2+2*w],t));
        return true;
    }
    if (!C::eval(p[-2-2*w],t)) {
        if (!C::eval(p[2-2*w],t) || !C::eval(p[3-w],t) || !C::eval(p[3],t) || !C::eval(p[3+w],t) || !C::eval(p[2+2*w],t) || !C::eval(p[1+w3],t) || !C::eval(p[w3],t) || !C::eval(p[-1+w3],t) || !C::eval(p[-2+2*w],t))
            return false;
        if (C::eval(p[-3+w],t)) return true;
        return (C::eval(p[1-w3],t)  && C::eval(p[-w3],t) && C::eval(p[-1-w3],t));
    }
    if (!C::eval(p[-1-w3],t)) {
        return (C::eval(p[3-w],t) && C::eval(p[3],t) && C::eval(p[3+w],t) && C::eval(p[2+2*w],t) && C::eval(p[1+w3],t) && C::eval(p[w3],t) && C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t));
    }
    if (!C::eval(p[-w3],t)) {
        return (C::eval(p[3],t) && C::eval(p[3+w],t) && C::eval(p[2+2*w],t) && C::eval(p[1+w3],t) && C::eval(p[w3],t) && C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
    }
    if (!C::eval(p[1-w3],t)) {
        return (C::eval(p[3+w],t) && C::eval(p[2+2*w],t) && C::eval(p[1+w3],t) && C::eval(p[w3],t) && C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
    }
    if (!C::eval(p[2-2*w],t)) {
        return (C::eval(p[2+2*w],t) && C::eval(p[1+w3],t) && C::eval(p[w3],t) && C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
    }
    if (!C::eval(p[3-w],t)) {
        return (C::eval(p[1+w3],t) && C::eval(p[w3],t) && C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
    }
    if (!C::eval(p[3],t)) {
        return (C::eval(p[w3],t) && C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
    }
    if (!C::eval(p[3+w],t)) {
        return (C::eval(p[-1+w3],t) && C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
    }
    if (!C::eval(p[2+2*w],t)) {
        return (C::eval(p[-2+2*w],t) && C::eval(p[-3+w],t));
    }
    if (!C::eval(p[1+w3],t)) {
        return (C::eval(p[-3+w],t));
    }
    return true;
}


template <int I, int N> struct BitCheck {
    template <class C> static inline void eval(unsigned int three, const byte* p, const int w, const int barrier, C& corners) {
	const int BIT = 1<<I;
	if (three & BIT) {
	    if (three & (BIT << 16)) {
		if (is_corner_12<Greater>(p, w, *p+barrier)) 
		    corners.push_back(p);
	    } else {
		if (is_corner_12<Less>(p, w, *p-barrier))
		    corners.push_back(p);
	    }
	}
	BitCheck<I+1,N>::eval(three, p+1, w, barrier, corners);
    }
};

template <int N> struct BitCheck<N,N> { 
    template <class C> static inline void eval(unsigned int three, const byte* p, const int w, const int barrier, C& corners) {} 
};

template <int CHUNKS, class C> inline void process_16(unsigned int three, const byte* p, const int w, const int barrier, C& corners)
{    
    three |= (three >> 16);
    const int BITS = 16/CHUNKS;
    const int mask = ((1<<BITS)-1);
    for (int i=0; i<CHUNKS; ++i) {
	if (three & mask)
	    BitCheck<0,BITS>::eval(three, p, w, barrier, corners);
	p += BITS;
	three >>= BITS;
    }
}

#define CHECK_BARRIER(here, other, flags)				\
    {									\
	__m128i diff = _mm_subs_epu8(here, other);			\
	__m128i diff2 = _mm_subs_epu8(other, here);			\
	asm( "pcmpgtb %%xmm7, %0  \n\t" : : "x"(diff2));		\
	asm( "pcmpgtb %%xmm7, %0  \n\t" : : "x"(diff));			\
	flags = _mm_movemask_epi8(diff) | (_mm_movemask_epi8(diff2) << 16); \
    }

void faster_corner_detect_12(const BasicImage<byte>& I, std::vector<ImageRef>& corners, int barrier)
{
    const int w = I.size().x;
    const int stride = 3*w;
    typedef std::list<const byte*> Passed;
    Passed passed;
    
    // Put the barrier in xmm7, which the compiler 
    // does not seem to allocate
    {   const __m128i barriers = _mm_set1_epi8((byte)barrier);
	asm( "movdqa %0, %%xmm7  \n\t" : : "x"(barriers));  }

    for (int i=3; i<I.size().y-3; ++i) {
	const byte* p = I[i];
	for (int j=0; j<w/16; ++j, p+=16) {
	    const __m128i here = _mm_load_si128((const __m128i*)(p));
	    const __m128i above = _mm_load_si128((const __m128i*)(p-stride));
	    const __m128i below = _mm_load_si128((const __m128i*)(p+stride));
	    unsigned int up_flags, down_flags;
	    CHECK_BARRIER(here, above, up_flags);
	    CHECK_BARRIER(here, below, down_flags);
	    const unsigned int either_ud = up_flags | down_flags;
	    if (either_ud) {
		unsigned int left_flags;
		{
		    const __m128i other = _mm_loadu_si128((const __m128i*)(p-3));
		    CHECK_BARRIER(here, other, left_flags);
		}
		const unsigned int both_ud = up_flags & down_flags;
		if (both_ud | (either_ud&left_flags)) {
		    unsigned int right_flags;
		    {
			const __m128i other = _mm_loadu_si128((const __m128i*)(p+3));
			CHECK_BARRIER(here, other, right_flags);
		    }
		    const unsigned int at_least_three = (either_ud & (left_flags & right_flags)) | (both_ud & (left_flags | right_flags));
		    if (at_least_three) {
			process_16<4>(at_least_three, p, w, barrier, passed);
		    }
		}
	    }
	}
	passed.push_back(0);
    }
    corners.reserve(passed.size());
    int row = 3;
    const byte* row_start = I[3];
    for (Passed::iterator it = passed.begin(); it != passed.end(); ++it) {
	if (*it == 0) {
	    row_start=I[++row];
	    continue;
	}
	int x = *it - row_start;
	if (x > 2 && x < w-3)
	    corners.push_back(ImageRef(x, row));
    }
}


}

#endif
