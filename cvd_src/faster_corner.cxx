#include <cvd/fast_corner.h>

#include <vector>
#include <list>
#include <cvd/utility.h>
using namespace CVD;
using namespace std;

#if (CVD_HAVE_EMMINTRIN && CVD_HAVE_SSE2)
#include <emmintrin.h>

#include "cvd_src/fast/prototypes.h"

namespace CVD
{
    #include "cvd_src/corner_9.h"    
    #include "cvd_src/corner_10.h"    
    #include "cvd_src/corner_12.h"    

    struct Less { template <class T1, class T2> static bool eval(const T1 a, const T2 b) { return a < b; }};
    struct Greater { template <class T1, class T2> static bool eval(const T1 a, const T2 b) { return b < a; }};

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

#define CHECK_BARRIER(lo, hi, other, flags)				\
    {									\
	__m128i diff = _mm_subs_epu8(lo, other);			\
	__m128i diff2 = _mm_subs_epu8(other, hi);			\
	__m128i z = _mm_setzero_si128();				\
	diff = _mm_cmpeq_epi8(diff, z);					\
	diff2 = _mm_cmpeq_epi8(diff2, z);				\
	flags = ~(_mm_movemask_epi8(diff) | (_mm_movemask_epi8(diff2) << 16)); \
    }
    
    
    template <bool Aligned> inline __m128i load_si128(const void* addr) { return _mm_loadu_si128((const __m128i*)addr); }
    template <> inline __m128i load_si128<true>(const void* addr) { return _mm_load_si128((const __m128i*)addr); }
    
    template <bool Aligned> void faster_corner_detect_12(const BasicImage<byte>& I, std::vector<ImageRef>& corners, int barrier)
    {
	const int w = I.size().x;
	const int stride = 3*w;
	typedef std::list<const byte*> Passed;
	Passed passed;
    
	// The compiler refuses to reserve a register for this,
	// even though xmm6 and xmm7 go unused.
	// It loads it from memory each time.  I am stymied.
	register const __m128i barriers = _mm_set1_epi8((byte)barrier);

	for (int i=3; i<I.size().y-3; ++i) {
	    const byte* p = I[i];
	    for (int j=0; j<w/16; ++j, p+=16) {
		__m128i lo, hi;
		{
		    const __m128i here = load_si128<Aligned>((const __m128i*)(p));
		    lo = _mm_subs_epu8(here, barriers);
		    hi = _mm_adds_epu8(barriers, here);
		}
		const __m128i above = load_si128<Aligned>((const __m128i*)(p-stride));
		const __m128i below = load_si128<Aligned>((const __m128i*)(p+stride));
		unsigned int up_flags, down_flags;
		CHECK_BARRIER(lo, hi, above, up_flags);
		CHECK_BARRIER(lo, hi, below, down_flags);
		const unsigned int either_ud = up_flags | down_flags;
		if (either_ud) {
		    unsigned int left_flags;
		    {
			const __m128i other = _mm_loadu_si128((const __m128i*)(p-3));
			CHECK_BARRIER(lo, hi, other, left_flags);
		    }
		    const unsigned int both_ud = up_flags & down_flags;
		    if (both_ud | (either_ud&left_flags)) {
			unsigned int right_flags;
			{
			    const __m128i other = _mm_loadu_si128((const __m128i*)(p+3));
			    CHECK_BARRIER(lo, hi, other, right_flags);
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

    void fast_corner_detect_12(const BasicImage<byte>& I, std::vector<ImageRef>& corners, int barrier)
    {
	if (I.size().x < 22) {
	    fast_corner_detect_plain_12(I,corners,barrier);
	    return;
	} else if (I.size().x < 22 || I.size().y < 7)
	    return;

	if (is_aligned<16>(I[0]) && is_aligned<16>(I[1]))
	    faster_corner_detect_12<true>(I, corners, barrier);
	else
	    faster_corner_detect_12<false>(I, corners, barrier);
    }

    template <bool Aligned> void faster_corner_detect_10(const BasicImage<byte>& I, std::vector<ImageRef>& corners, const int barrier)
    {
	const int w = I.size().x;
	const int stride = 3*w;
	typedef std::list<std::pair<const byte*, unsigned int> > Passed;
	Passed passed;
 
	// The compiler refuses to reserve a register for this
	register const __m128i barriers = _mm_set1_epi8((byte)barrier);
	const byte* const end = I[I.size().y - 3]-16;

	for (const byte* p = I[3] + 16; p!=end; p+=16) {
	    __m128i lo, hi;
	    {
		const __m128i here = load_si128<Aligned>((const __m128i*)(p));
		lo = _mm_subs_epu8(here, barriers);
		hi = _mm_adds_epu8(barriers, here);
	    }
	    unsigned int ans_b, ans_e;
	    {
		__m128i top = load_si128<Aligned>((const __m128i*)(p-stride));
		__m128i bottom = load_si128<Aligned>((const __m128i*)(p+stride));

		CHECK_BARRIER(lo, hi, top, ans_b);
		CHECK_BARRIER(lo, hi, bottom, ans_e);
		if (!(ans_b | ans_e))
		    continue;	 
	    }

	    unsigned int ans_m, ans_p, possible;
	    {
		__m128i ul = _mm_loadu_si128((const __m128i*)(p-2-2*w));
		__m128i lr = _mm_loadu_si128((const __m128i*)(p+2+2*w));
		CHECK_BARRIER(lo, hi, ul, ans_m);
		CHECK_BARRIER(lo, hi, lr, ans_p);
		possible = (ans_m & ans_b) | (ans_e & ans_p);
		if (!possible)
		    continue;
	    }

	    unsigned int ans_o, ans_n;
	    {
		__m128i ll = _mm_loadu_si128((const __m128i*)(p-2+2*w));
		__m128i ur = _mm_loadu_si128((const __m128i*)(p+2-2*w));
		CHECK_BARRIER(lo, hi, ll, ans_o);
		CHECK_BARRIER(lo, hi, ur, ans_n);
		possible &= ans_o | (ans_b & ans_n);
		possible &= ans_n | (ans_e & ans_o);
		if (!possible)
		    continue;
	    }
 
	    unsigned int ans_h, ans_k;
	    {
		__m128i left = _mm_loadu_si128((const __m128i*)(p-3));
		__m128i right = _mm_loadu_si128((const __m128i*)(p+3));
		CHECK_BARRIER(lo, hi, left, ans_h);
		CHECK_BARRIER(lo, hi, right, ans_k);
		possible &= ans_h | (ans_n & ans_k & ans_p);
		possible &= ans_k | (ans_m & ans_h & ans_o);
		if (!possible)
		    continue;
	    }
	    
	    unsigned int ans_a, ans_c;
	    {
		__m128i a = _mm_loadu_si128((const __m128i*)(p-1-stride));
		__m128i c = _mm_insert_epi16(_mm_srli_si128(a,2), *(const unsigned short*)(p+15-stride), 7);
		//__m128i c = _mm_loadu_si128((const __m128i*)(p+1-stride));
		CHECK_BARRIER(lo, hi, a, ans_a);
		CHECK_BARRIER(lo, hi, c, ans_c);
		possible &= ans_a | (ans_e & ans_p);
		possible &= ans_c | (ans_o & ans_e);
		if (!possible)
		    continue;
	    }

	    unsigned int ans_d, ans_f;
	    {
		__m128i d = _mm_loadu_si128((const __m128i*)(p-1+stride));
		__m128i f = _mm_insert_epi16(_mm_srli_si128(d,2), *(const unsigned short*)(p+15+stride), 7);
		//__m128i f = _mm_loadu_si128((const __m128i*)(p+1+stride));
		CHECK_BARRIER(lo, hi, d, ans_d);
		CHECK_BARRIER(lo, hi, f, ans_f);
		const unsigned int ans_abc = ans_a & ans_b & ans_c;
		possible &= ans_d | (ans_abc & ans_n);
		possible &= ans_f | (ans_m & ans_abc);
		if (!possible)
		    continue;
	    }

	    unsigned int ans_g, ans_i;
	    {
		__m128i g = _mm_loadu_si128((const __m128i*)(p-3-w));
		__m128i ii = _mm_loadu_si128((const __m128i*)(p-3+w));
		CHECK_BARRIER(lo, hi, g, ans_g);
		CHECK_BARRIER(lo, hi, ii, ans_i);
		possible &= ans_g | (ans_f & ans_p & ans_k);
		possible &= ans_i | (ans_c & ans_n & ans_k);
		if (!possible)
		    continue;
	    }

	    unsigned int ans_j, ans_l;
	    {
		__m128i jj = _mm_loadu_si128((const __m128i*)(p+3-w));
		__m128i l = _mm_loadu_si128((const __m128i*)(p+3+w));
		CHECK_BARRIER(lo, hi, jj, ans_j);
		CHECK_BARRIER(lo, hi, l, ans_l);
		const unsigned int ans_ghi = ans_g & ans_h & ans_i;
		possible &= ans_j | (ans_d & ans_o & ans_ghi);
		possible &= ans_l | (ans_m & ans_a & ans_ghi);
		if (!possible)
		    continue;
	    }
	    passed.push_back(make_pair(p,(possible | (possible>>16))&0xFFFF));
	}
	corners.reserve(passed.size());
	int row = 3;
	const byte* row_start = I[3];
	// Check first 16
	{
	    for (int j=3; j<16; ++j)
		if (is_corner_10<Less>(row_start + j, w, row_start[j]-barrier) || 
		    is_corner_10<Greater>(row_start + j, w, row_start[j]+barrier))
		    corners.push_back(ImageRef(j,3));
	}
	for (Passed::iterator it = passed.begin(); it != passed.end(); ++it) {
	    while (it->first >= row_start + w) {
		++row;
		row_start += w;
	    }
	    int x = it->first - row_start;
	    unsigned int bits = it->second;
	    if (x == 0) {
		x = 3;
		bits >>= 3;
	    } else if (x >= w-18)
		bits &= 0x1FFF;
	    for (;bits;bits>>=1, ++x) {
		if (bits&0x1)
		    corners.push_back(ImageRef(x,row));
	    }
	}
	// Check last 16
	{
	    row_start = I[I.size().y-4];
	    for (int j=w-16; j<w-3; ++j)
		if (is_corner_10<Less>(row_start + j, w, row_start[j]-barrier) || 
		    is_corner_10<Greater>(row_start + j, w, row_start[j]+barrier))
		    corners.push_back(ImageRef(j,I.size().y-4));
	}
    }

    void fast_corner_detect_10(const BasicImage<byte>& I, std::vector<ImageRef>& corners, int barrier)
    {
	if (I.size().x < 22) {
	    fast_corner_detect_plain_10(I,corners,barrier);
	    return;
	} else if (I.size().x < 22 || I.size().y < 7)
	    return;

	if (is_aligned<16>(I[0]) && is_aligned<16>(I[1]))
	    faster_corner_detect_10<true>(I, corners, barrier);
	else
	    faster_corner_detect_10<false>(I, corners, barrier);
    }

    template <bool Aligned> void faster_corner_detect_9(const BasicImage<byte>& I, std::vector<ImageRef>& corners, const int barrier)
    {
	const int w = I.size().x;
	const int stride = 3*w;
	typedef std::list<std::pair<const byte*, unsigned int> > Passed;
	Passed passed;
	// The compiler refuses to reserve a register for this
	register const __m128i barriers = _mm_set1_epi8((byte)barrier);
	const byte* const end = I[I.size().y - 3]-16;

	for (const byte* p = I[3] + 16; p!=end; p+=16) {
	    __m128i lo, hi;
	    {
		const __m128i here = load_si128<Aligned>((const __m128i*)(p));
		lo = _mm_subs_epu8(here, barriers);
		hi = _mm_adds_epu8(barriers, here);
	    }
	    unsigned int ans_0, ans_8, possible;
	    {
		__m128i top = load_si128<Aligned>((const __m128i*)(p-stride));
		__m128i bottom = load_si128<Aligned>((const __m128i*)(p+stride));

		CHECK_BARRIER(lo, hi, top, ans_0);
		CHECK_BARRIER(lo, hi, bottom, ans_8);
		possible = ans_0 | ans_8;
		if (!possible)
		    continue;	 
	    }

	    unsigned int ans_15, ans_1;
	    {
		__m128i a = _mm_loadu_si128((const __m128i*)(p-1-stride));
		__m128i c = _mm_insert_epi16(_mm_srli_si128(a,2), *(const unsigned short*)(p+15-stride), 7);
		CHECK_BARRIER(lo, hi, a, ans_15);
		CHECK_BARRIER(lo, hi, c, ans_1);
		possible &= ans_8 | (ans_15 & ans_1);
		if (!possible)
		    continue;
	    }

	    unsigned int ans_9, ans_7;
	    {
		__m128i d = _mm_loadu_si128((const __m128i*)(p-1+stride));
		__m128i f = _mm_insert_epi16(_mm_srli_si128(d,2), *(const unsigned short*)(p+15+stride), 7);
		CHECK_BARRIER(lo, hi, d, ans_9);
		CHECK_BARRIER(lo, hi, f, ans_7);
		possible &= ans_9 | (ans_0 & ans_1);
		possible &= ans_7 | (ans_15 & ans_0);
		if (!possible)
		    continue;
	    }

	    unsigned int ans_12, ans_4;
	    {
		__m128i left = _mm_loadu_si128((const __m128i*)(p-3));
		__m128i right = _mm_loadu_si128((const __m128i*)(p+3));
		CHECK_BARRIER(lo, hi, left, ans_12);
		CHECK_BARRIER(lo, hi, right, ans_4);
		possible &= ans_12 | (ans_4 & (ans_1 | ans_7));
		possible &= ans_4 | (ans_12 & (ans_9 | ans_15));
		if (!possible)
		    continue;
	    }

	    unsigned int ans_14, ans_6;
	    {
		__m128i ul = _mm_loadu_si128((const __m128i*)(p-2-2*w));
		__m128i lr = _mm_loadu_si128((const __m128i*)(p+2+2*w));
		CHECK_BARRIER(lo, hi, ul, ans_14);
		CHECK_BARRIER(lo, hi, lr, ans_6);
		{
		    const unsigned int ans_6_7 = ans_6 & ans_7;
		    possible &= ans_14 | (ans_6_7 & (ans_4 | (ans_8 & ans_9)));
		    possible &= ans_1 | (ans_6_7) | ans_12;
		}
		{
		    const unsigned int ans_14_15 = ans_14 & ans_15;
		    possible &= ans_6 | (ans_14_15 & (ans_12 | (ans_0 & ans_1)));
		    possible &= ans_9 | (ans_14_15) | ans_4;
		}
		if (!possible)
		    continue;
	    }

	    unsigned int ans_10, ans_2;
	    {
		__m128i ll = _mm_loadu_si128((const __m128i*)(p-2+2*w));
		__m128i ur = _mm_loadu_si128((const __m128i*)(p+2-2*w));
		CHECK_BARRIER(lo, hi, ll, ans_10);
		CHECK_BARRIER(lo, hi, ur, ans_2);
		{
		    const unsigned int ans_1_2 = ans_1 & ans_2;
		    possible &= ans_10 | (ans_1_2 & ((ans_0 & ans_15) | ans_4));
		    possible &= ans_12 | (ans_1_2) | (ans_6 & ans_7);
		}
		{
		    const unsigned int ans_9_10 = ans_9 & ans_10;
		    possible &= ans_2 | (ans_9_10 & ((ans_7 & ans_8) | ans_12));
		    possible &= ans_4 | (ans_9_10) | (ans_14 & ans_15);
		}
		possible &= ans_8 | ans_14 | ans_2;
		possible &= ans_0 | ans_10 | ans_6;
		if (!possible)
		    continue;
	    }

	    unsigned int ans_13, ans_5;
	    {
		__m128i g = _mm_loadu_si128((const __m128i*)(p-3-w));
		__m128i l = _mm_loadu_si128((const __m128i*)(p+3+w));
		CHECK_BARRIER(lo, hi, g, ans_13);
		CHECK_BARRIER(lo, hi, l, ans_5);
		const unsigned int ans_15_0 = ans_15 & ans_0; 
		const unsigned int ans_7_8 = ans_7 & ans_8;
		{
		    const unsigned int ans_12_13 = ans_12 & ans_13;
		    possible &= ans_5 | (ans_12_13 & ans_14 & ((ans_15_0) | ans_10));
		    possible &= ans_7 | (ans_1 & ans_2) | (ans_12_13);
		    possible &= ans_2 | (ans_12_13) | (ans_7_8);
		}
		{
		    const unsigned int ans_4_5 = ans_4 & ans_5;
		    const unsigned int ans_9_10 = ans_9 & ans_10;
		    possible &= ans_13 | (ans_4_5 & ans_6 & ((ans_7_8) | ans_2));
		    possible &= ans_15 | (ans_4_5) | (ans_9_10);
		    possible &= ans_10 | (ans_4_5) | (ans_15_0);
		    possible &= ans_15 | (ans_9_10) | (ans_4_5);
		}

		possible &= ans_8 | (ans_13 & ans_14) | ans_2;
		possible &= ans_0 | (ans_5 & ans_6) | ans_10;
		if (!possible)
		    continue;
	    }


	    unsigned int ans_11, ans_3;
	    {
		__m128i ii = _mm_loadu_si128((const __m128i*)(p-3+w));
		__m128i jj = _mm_loadu_si128((const __m128i*)(p+3-w));
		CHECK_BARRIER(lo, hi, ii, ans_11);
		CHECK_BARRIER(lo, hi, jj, ans_3);
		{
		    const unsigned int ans_2_3 = ans_2 & ans_3;
		    possible &= ans_11 | (ans_2_3 & ans_4 & ((ans_0 & ans_1) | (ans_5 & ans_6)));
		    possible &= ans_13 | (ans_7 & ans_8) | (ans_2_3);
		    possible &= ans_8 | (ans_2_3) | (ans_13 & ans_14);
		}
		{
		    const unsigned int ans_11_12 = ans_11 & ans_12;
		    possible &= ans_3 | (ans_10 & ans_11_12 & ((ans_8 & ans_9) | (ans_13 & ans_14)));
		    possible &= ans_1 | (ans_11_12) | (ans_6 & ans_7);
		    possible &= ans_6 | (ans_0 & ans_1) | (ans_11_12);
		}
		{
		    const unsigned int ans_3_4 = ans_3 & ans_4;
		    possible &= ans_9 | (ans_3_4) | (ans_14 & ans_15);
		    possible &= ans_14 | (ans_8 & ans_9) | (ans_3_4);
		}
		{
		    const unsigned int ans_10_11 = ans_10 & ans_11;
		    possible &= ans_5 | (ans_15 & ans_0) | (ans_10_11);
		    possible &= ans_0 | (ans_10_11) | (ans_5 & ans_6);
		}
		if (!possible)
		    continue;

	    }
	    passed.push_back(make_pair(p,(possible | (possible>>16))&0xFFFF));
	}
	corners.reserve(passed.size()*2);
	int row = 3;
	const byte* row_start = I[3];
	// Check first 16
	{
	    for (int j=3; j<16; ++j)
		if (is_corner_9<Less>(row_start + j, w, row_start[j]-barrier) || 
		    is_corner_9<Greater>(row_start + j, w, row_start[j]+barrier))
		    corners.push_back(ImageRef(j,3));
	}
	for (Passed::iterator it = passed.begin(); it != passed.end(); ++it) {
	    while (it->first >= row_start + w) {
		++row;
		row_start += w;
	    }
	    int x = it->first - row_start;
	    unsigned int bits = it->second;
	    if (x == 0) {
		x = 3;
		bits >>= 3;
	    } else if (x >= w-18)
		bits &= 0x1FFF;
	    for (;bits;bits>>=1, ++x) {
		if (bits&0x1)
		    corners.push_back(ImageRef(x,row));
	    }
	}
	// Check last 16
	{
	    row_start = I[I.size().y-4];
	    for (int j=w-16; j<w-3; ++j)
		if (is_corner_9<Less>(row_start + j, w, row_start[j]-barrier) || 
		    is_corner_9<Greater>(row_start + j, w, row_start[j]+barrier))
		    corners.push_back(ImageRef(j,I.size().y-4));
	}
    }

    void fast_corner_detect_9(const BasicImage<byte>& I, std::vector<ImageRef>& corners, int barrier)
    {
	if (I.size().x < 22) {
	    fast_corner_detect_plain_9(I,corners,barrier);
	    return;
	} else if (I.size().x < 22 || I.size().y < 7)
	    return;

	if (is_aligned<16>(I[0]) && is_aligned<16>(I[1]))
	    faster_corner_detect_9<true>(I, corners, barrier);
	else
	    faster_corner_detect_9<false>(I, corners, barrier);
    }

}

#endif
