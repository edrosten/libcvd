#include <cvd/config.h>
#include <cvd/utility.h>
// internal functions used by CVD vision algorithm implementations
#include <cvd/internal/assembly.h>

#if CVD_HAVE_MMINTRIN
#    include <mmintrin.h>
#endif

#if CVD_HAVE_XMMINTRIN
#    include <xmmintrin.h>
#endif

#if CVD_HAVE_EMMINTRIN
#    include <emmintrin.h>
#endif

using namespace std;

    using CVD::is_aligned;
    using CVD::steps_to_align;
    template <class F, class T1, class T2, int A, int M> inline void maybe_aligned_differences(const T1* a, const T1* b, T2* c, unsigned int count)
    {
	if (count < M*2) {
	    F::unaligned_differences(a,b,c,count);
	    return;
	}
	if (!is_aligned<A>(a)) {	    
	    unsigned int steps = steps_to_align<A>(a);
	    F::unaligned_differences(a,b,c,steps);
	    count -= steps;
	    a += steps;
	    b += steps;
	    c += steps;
	}
	if (!is_aligned<A>(c) || count < M) {
	    F::unaligned_differences(a,b,c,count);
	    return;
	}	
	unsigned int block = (count/M)*M;
	F::aligned_differences(a,b,c,block);
	if (count > block) {
	    F::unaligned_differences(a+block,b+block,c+block,count-block);
	}
    }    
    
    template <class F, class T1, class T2, int A, int M> inline void maybe_aligned_add_mul_add(const T1* a, const T1* b, const T1& c, T2* out, unsigned int count)
    {
	if (count < M*2) {
	    F::unaligned_add_mul_add(a,b,c,out,count);
	    return;
	}
	if (!is_aligned<A>(a)) {      
	    unsigned int steps = steps_to_align<A>(a);
	    F::unaligned_add_mul_add(a,b,c,out,steps);
	    count -= steps;
	    a += steps;
	    b += steps;
	    out += steps;
	    if (count < M || !is_aligned<16>(out)) {
		F::unaligned_add_mul_add(a,b,c,out,count);
		return;
	    }
	}
	else if (count < M || !is_aligned<16>(out)) {
	    F::unaligned_add_mul_add(a,b,c,out,count);
	    return;
	}
	unsigned int block = (count/M)*M;
	F::aligned_add_mul_add(a,b,c,out,block);
	if (count > block)
	    F::unaligned_add_mul_add(a+block,b+block,c, out+block,count-block);
    }    

    template <class F, class T1, class T2, int A, int M> inline void maybe_aligned_assign_mul(const T1* a, const T1& c, T2* out, unsigned int count)
    {
	if (count < M*2) {
	    F::unaligned_assign_mul(a,c,out,count);
	    return;
	}
	if (!is_aligned<A>(a)) {      
	    unsigned int steps = steps_to_align<A>(a);
	    F::unaligned_assign_mul(a,c,out,steps);
	    count -= steps;
	    a += steps;
	    out += steps;
	    if (count < M) {
		F::unaligned_assign_mul(a,c,out,count);
		return;
	    }
	}
	unsigned int block = (count/M)*M;
	F::aligned_assign_mul(a,c,out,block);
	if (count > block) {
	    F::unaligned_assign_mul(a+block,c, out+block,count-block);
	}
    }    

    template <class F, class R, class T1, int A, int M> inline R maybe_aligned_inner_product(const T1* a, const T1* b, unsigned int count)
    {
	if (count < M*2) {
	    return F::unaligned_inner_product(a,b,count);
	}
	R sum = 0;
	if (!is_aligned<A>(a)) {      
	    unsigned int steps = steps_to_align<A>(a);
	    sum = F::unaligned_inner_product(a,b,steps);
	    count -= steps;
	    a += steps;
	    b += steps;
	    if (count < M) {
		return sum + F::unaligned_inner_product(a,b,count);
	    }
	}
	unsigned int block = (count/M)*M;
	sum += F::aligned_inner_product(a,b,block);
	if (count > block)
	    sum += F::unaligned_inner_product(a+block,b+block,count-block);
	return sum;
    }    

    template <class F, class R, class T1, int A, int M> inline R maybe_aligned_ssd(const T1* a, const T1* b, unsigned int count)
    {
	if (count < M*2) {
	    return F::unaligned_ssd(a,b,count);
	}
	R sum = 0;
	if (!is_aligned<A>(a)) {      
	    unsigned int steps = steps_to_align<A>(a);
	    sum = F::unaligned_ssd(a,b,steps);
	    count -= steps;
	    a += steps;
	    b += steps;
	    if (count < M) {
		return sum + F::unaligned_ssd(a,b,count);
	    }
	}
	unsigned int block = (count/M)*M;
	sum += F::aligned_ssd(a,b,block);
	if (count > block)
	    sum += F::unaligned_ssd(a+block,b+block,count-block);
	return sum;
    }    



namespace CVD {

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_MMINTRIN)


    void byte_to_short_differences(const __m64* a, const __m64* b, __m64* diff, unsigned int count)
    {
	__m64 z = _mm_setzero_si64();
	for (;count; --count, ++a, ++b, diff+=2) {
	    __m64 aq = *a;
	    __m64 bq = *b;
	    __m64 alo = _mm_unpacklo_pi8(aq,z);
	    __m64 ahi = _mm_unpackhi_pi8(aq,z);
	    __m64 blo = _mm_unpacklo_pi8(bq,z);
	    __m64 bhi = _mm_unpackhi_pi8(bq,z);
	    diff[0] = _mm_sub_pi16(alo,blo);
	    diff[1] = _mm_sub_pi16(ahi,bhi);
	}
	_mm_empty();
    }

    void short_differences(const __m64* a, const __m64* b, __m64* diff, unsigned int count)
    {
	while (count--) {
	    *(diff++) = _mm_sub_pi16(*(a++), *(b++));
	}
	_mm_empty();
    }

    
    struct MMX_funcs {
	template <class T1, class T2> static inline void unaligned_differences(const T1* a, const T1* b, T2* diff, size_t count) {
	    differences<T1,T2>(a,b,diff,count);
	}
	static inline void aligned_differences(const byte* a, const byte* b, short* diff, unsigned int count) {
	    if (is_aligned<8>(b))
		byte_to_short_differences((const __m64*)a,(const __m64*)b, (__m64*)diff, count>>3);
	    else
		unaligned_differences(a,b,diff,count);
	}
	
	static inline void aligned_differences(const short* a, const short* b, short* diff, unsigned int count) {
	    if (is_aligned<8>(b))	    
		short_differences((const __m64*)a, (const __m64*)b, (__m64*)diff, count>>2);
	    else
		unaligned_differences(a,b,diff,count);
	}
    };

    void differences(const byte* a, const byte* b, short* diff, unsigned int count) {
	maybe_aligned_differences<MMX_funcs, byte, short, 8, 8>(a,b,diff,count);
    }
    
    void differences(const short* a, const short* b, short* diff, unsigned int count) {
	maybe_aligned_differences<MMX_funcs, short, short, 8, 4>(a,b,diff,count);
    }
#endif


#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_XMMINTRIN)

    template <bool Aligned> inline __m128 load_ps(const void* addr) { return _mm_loadu_ps((const float*)addr); }
    template <> inline __m128 load_ps<true>(const void* addr) { return _mm_load_ps((const float*)addr); }

    template <bool Aligned> inline void store_ps(__m128 m, void* addr) { return _mm_storeu_ps((float*)addr, m); }
    template <> inline void store_ps<true>(__m128 m, void* addr) { return _mm_store_ps((float*)addr, m); }

    template <bool Aligned_b> inline void float_differences(const __m128* a, const __m128* b, __m128* diff, unsigned int count)
    {
	while (count--) {
	    *(diff++) = _mm_sub_ps(*(a++), load_ps<Aligned_b>(b++));
	}
    }
    
    template <bool Aligned_b> void float_add_multiple_of_sum(const __m128* a, const __m128* b, const float& c, __m128* out, unsigned int count)
    {
	__m128 cccc = _mm_set1_ps(c);
	while (count--) {
	    *out = _mm_add_ps(_mm_mul_ps(_mm_add_ps(*(a++), load_ps<Aligned_b>(b++)), cccc), *out);
	    ++out;
	}
    }

    template <bool Aligned_out> inline void float_assign_multiple(const __m128* a, const float& c, __m128* out, unsigned int count)
    {
	const __m128 cccc = _mm_set1_ps(c);
	while (count--)
	    store_ps<Aligned_out>(_mm_mul_ps(*(a++), cccc), out++);
       
    }
    
    template <bool Aligned_b> double float_inner_product(const __m128* a, const __m128* b, unsigned int count)
    {
	float sums_store[4];
	const unsigned int BLOCK = 1<<10;
	double dot = 0;
	while (count) {
	    size_t pass = std::min(count, BLOCK);
	    count-=pass;
	    __m128 sums = _mm_setzero_ps();
	    while (pass--) {
		__m128 prod = _mm_mul_ps(*(a++), load_ps<Aligned_b>(b++));
		sums = _mm_add_ps(prod, sums);
	    }
	    _mm_storeu_ps(sums_store, sums);
	    dot += sums_store[0] + sums_store[1] + sums_store[2] + sums_store[3];
	}
	return dot;
    }

    template <bool Aligned_b> inline double float_sum_squared_differences(const __m128* a, const __m128* b, size_t count) 
    {
	float sums_store[4];
	const size_t BLOCK = 1<<10;
	double ssd = 0;
	while (count) {
	    size_t pass = std::min(count, BLOCK);
	    count-=pass;
	    __m128 sums = _mm_setzero_ps();
	    while (pass--) {
		__m128 diff = _mm_sub_ps(*(a++), load_ps<Aligned_b>(b++));
		sums = _mm_add_ps(_mm_mul_ps(diff,diff), sums);
	    }
	    _mm_storeu_ps(sums_store, sums);
	    ssd += sums_store[0] + sums_store[1] + sums_store[2] + sums_store[3];
	}
	return ssd;
    }

    struct SSE_funcs {
	template <class T1, class T2> static inline void unaligned_differences(const T1* a, const T1* b, T2* diff, size_t count) {
	    differences<T1,T2>(a,b,diff,count);
	}
	
	static inline void aligned_differences(const float* a, const float* b, float* diff, unsigned int count) {
	    if (is_aligned<16>(b))
		float_differences<true>((const __m128*)a, (const __m128*)b, (__m128*)diff, count>>2);
	    else
		float_differences<false>((const __m128*)a, (const __m128*)b, (__m128*)diff, count>>2);
	}

	template <class T1, class T2> static inline void unaligned_add_mul_add(const T1* a, const T1* b, const T1& c, T2* out, size_t count) {
	    add_multiple_of_sum<T1,T2>(a,b,c,out,count);
	}
	static inline void aligned_add_mul_add(const float* a, const float* b, const float& c, float* out, size_t count) {
	    if (is_aligned<16>(b))
		float_add_multiple_of_sum<true>((const __m128*)a, (const __m128*)b, c, (__m128*)out, count>>2);
	    else
		float_add_multiple_of_sum<false>((const __m128*)a, (const __m128*)b, c, (__m128*)out, count>>2);
	}	

	template <class T1, class T2> static inline void unaligned_assign_mul(const T1* a, const T1& c, T2* out, size_t count) {
	    assign_multiple<T1,T2>(a,c,out,count);
	}
	static inline void aligned_assign_mul(const float* a, const float& c, float* out, size_t count) {
	    if (is_aligned<16>(out)) 
		float_assign_multiple<false>((const __m128*)a, c, (__m128*)out, count>>2);
	    else		
		float_assign_multiple<false>((const __m128*)a, c, (__m128*)out, count>>2);
	}	

	template <class T1> static inline double unaligned_inner_product(const T1* a, const T1* b, size_t count) {
	    return inner_product<T1>(a,b,count);
	}
	
	static inline double aligned_inner_product(const float* a, const float* b, unsigned int count)
	{
	    if (is_aligned<16>(b))
		return float_inner_product<true>((const __m128*) a, (const __m128*) b, count>>2);
	    else
		return float_inner_product<false>((const __m128*) a, (const __m128*) b, count>>2);
	}	

	template <class T1> static inline double unaligned_ssd(const T1* a, const T1* b, size_t count) {
	    return sum_squared_differences<T1>(a,b,count);
	}
	
	static inline double aligned_ssd(const float* a, const float* b, unsigned int count)
	{
	    if (is_aligned<16>(b))
		return float_sum_squared_differences<true>((const __m128*) a, (const __m128*) b, count>>2);
	    else
		return float_sum_squared_differences<false>((const __m128*) a, (const __m128*) b, count>>2);
	}	
    };
    
    void differences(const float* a, const float* b, float* diff, unsigned int size)
    {
	maybe_aligned_differences<SSE_funcs, float, float, 16, 4>(a,b,diff,size);
    }
    
    void add_multiple_of_sum(const float* a, const float* b, const float& c,  float* out, unsigned int count)
    {
	maybe_aligned_add_mul_add<SSE_funcs,float,float,16,4>(a,b,c,out,count);
    }
    
    void assign_multiple(const float* a, const float& c,  float* out, unsigned int count) 
    {
	maybe_aligned_assign_mul<SSE_funcs,float,float,16,4>(a,c,out,count);
    }

    
    double inner_product(const float* a, const float* b, unsigned int count) 
    {
	return maybe_aligned_inner_product<SSE_funcs,double,float,16,4>(a,b,count);
    }

    double sum_squared_differences(const float* a, const float* b, size_t count)
    {
	return maybe_aligned_ssd<SSE_funcs,double,float,16,4>(a,b,count);
    }

#endif

#if defined (CVD_HAVE_SSE2) && defined(CVD_HAVE_EMMINTRIN)


    static inline __m128i zero_si128() { __m128i x; asm ( "pxor %0, %0  \n\t" : "=x"(x) ); return x; }
    template <bool Aligned> inline __m128i load_si128(const void* addr) { return _mm_loadu_si128((const __m128i*)addr); }
    template <> inline __m128i load_si128<true>(const void* addr) { return _mm_load_si128((const __m128i*)addr); }
    template <bool Aligned> inline __m128d load_pd(const void* addr) { return _mm_loadu_pd((const double*)addr); }
    template <> inline __m128d load_pd<true>(const void* addr) { return _mm_load_pd((const double*)addr); }

    template <bool Aligned> inline void store_pd(__m128d m, void* addr) { return _mm_storeu_pd((double*)addr, m); }
    template <> inline void store_pd<true>(__m128d m, void* addr) { return _mm_store_pd((double*)addr, m); }

    template <bool Aligned_b> void int_differences(const __m128i* a, const __m128i* b, __m128i* diff, unsigned int count)
    {
	while (count--) {
	    *(diff++) = _mm_sub_epi32(*(a++), load_si128<Aligned_b>(b++));
	}
    }
    
    template <bool Aligned_b> void double_differences(const __m128d* a, const __m128d* b, __m128d* diff, unsigned int count)
    {
	while (count--) {
	    *(diff++) = _mm_sub_pd(*(a++), load_pd<Aligned_b>(b++));
	}
    }

    template <bool Aligned_b> void double_add_multiple_of_sum(const __m128d* a, const __m128d* b, const double& c, __m128d* out, unsigned int count)
    {
	__m128d cc = _mm_set1_pd(c);
	while (count--) {
	    *out = _mm_add_pd(_mm_mul_pd(_mm_add_pd(*(a++), load_pd<Aligned_b>(b++)), cc), *out);
	    ++out;
	}
    }

    template <bool Aligned_out> void double_assign_multiple(const __m128d* a, const double& c, __m128d* out, unsigned int count)
    {
	__m128d cc = _mm_set1_pd(c);
	while (count--)
	    store_pd<Aligned_out>(_mm_mul_pd(*(a++), cc), out++);
    }
    template <bool Aligned_b> double double_inner_product(const __m128d* a, const __m128d* b, unsigned int count)
    {
	double sums_store[2];
	const unsigned int BLOCK = 1<<16;
	double dot = 0;
	while (count) {
	    size_t pass = std::min(count, BLOCK);
	    count-=pass;
	    __m128d sums = _mm_setzero_pd();
	    while (pass--) {
		__m128d prod = _mm_mul_pd(*(a++), load_pd<Aligned_b>(b++));
		sums = _mm_add_pd(prod, sums);
	    }
	    _mm_storeu_pd(sums_store, sums);
	    dot += sums_store[0] + sums_store[1];
	}
	return dot;
    }

    template <bool Aligned_b> long long byte_sum_squared_differences(const __m128i* a, const __m128i* b, unsigned int count) {
	unsigned long sums_store[4];	
	const unsigned int BLOCK = 1<<15;
	long long ssd = 0;
	while (count) {
	    size_t pass = std::min(count, BLOCK);
	    count -= pass;
	    __m128i sums = _mm_setzero_si128();
	    while (pass--) {
		__m128i lo_a = load_si128<true>(a++);
		__m128i lo_b = load_si128<Aligned_b>(b++);
		__m128i hi_a = _mm_unpackhi_epi8(lo_a, sums);
		__m128i hi_b = _mm_unpackhi_epi8(lo_b, sums);
		lo_a = _mm_unpacklo_epi8(lo_a, sums);
		lo_b = _mm_unpacklo_epi8(lo_b, sums);
		lo_a = _mm_sub_epi16(lo_a, lo_b);
		hi_a = _mm_sub_epi16(hi_a, hi_b);
		lo_a = _mm_madd_epi16(lo_a,lo_a);
		hi_a = _mm_madd_epi16(hi_a,hi_a);
		sums = _mm_add_epi32(_mm_add_epi32(lo_a, hi_a), sums);
	    }
	    _mm_storeu_si128((__m128i*)sums_store, sums);
	    ssd += sums_store[0] + sums_store[1] + sums_store[2] + sums_store[3];
	}
	return ssd;
    }

    template <bool Aligned_b> inline double double_sum_squared_differences(const __m128d* a, const __m128d* b, unsigned int count) 
    {
	double sums_store[2];
	const unsigned int BLOCK = 1<<10;
	double ssd = 0;
	while (count) {
	    size_t pass = std::min(count, BLOCK);
	    count-=pass;
	    __m128d sums = _mm_setzero_pd();
	    while (pass--) {
		__m128d diff = _mm_sub_pd(*(a++), load_pd<Aligned_b>(b++));
		sums = _mm_add_pd(_mm_mul_pd(diff,diff), sums);
	    }
	    _mm_storeu_pd(sums_store, sums);
	    ssd += sums_store[0] + sums_store[1];
	}
	return ssd;
    }

    
    struct SSE2_funcs {
	template <class T1, class T2> static inline void unaligned_differences(const T1* a, const T1* b, T2* diff, size_t count) {
	    differences<T1,T2>(a,b,diff,count);
	}

	static inline void aligned_differences(const int32_t* a, const int32_t* b, int32_t* diff, unsigned int count) {
	    if (is_aligned<16>(b))
		int_differences<true>((const __m128i*)a, (const __m128i*)b, (__m128i*)diff, count>>2);
	    else
		int_differences<false>((const __m128i*)a, (const __m128i*)b, (__m128i*)diff, count>>2);
	}

	static inline void aligned_differences(const double* a, const double* b, double* diff, unsigned int count)
	{
	    if (is_aligned<16>(b))
		double_differences<true>((const __m128d*)a,(const __m128d*)b,(__m128d*)diff,count>>1);
	    else
		double_differences<false>((const __m128d*)a,(const __m128d*)b,(__m128d*)diff,count>>1);
	}

	template <class T1, class T2> static inline void unaligned_add_mul_add(const T1* a, const T1* b, const T1& c, T2* out, size_t count) {
	    add_multiple_of_sum<T1,T2>(a,b,c,out,count);
	}
	
	static inline void aligned_add_mul_add(const double* a, const double* b, const double& c, double* out, unsigned int count)
	{
	    if (is_aligned<16>(b))
		double_add_multiple_of_sum<true>((const __m128d*)a, (const __m128d*)b, c, (__m128d*)out, count>>1);
	    else
		double_add_multiple_of_sum<false>((const __m128d*)a, (const __m128d*)b, c, (__m128d*)out, count>>1);
	}
	
	template <class T1, class T2> static inline void unaligned_assign_mul(const T1* a, const T1& c, T2* out, size_t count) {
	    assign_multiple<T1,T2>(a,c,out,count);
	}

	static inline void aligned_assign_mul(const double* a, const double& c, double* out, unsigned int count)
	{
	    if (is_aligned<16>(out))
		double_assign_multiple<true>((const __m128d*)a, c, (__m128d*)out, count>>1);
	    else
		double_assign_multiple<false>((const __m128d*)a, c, (__m128d*)out, count>>1);
	}
	
	template <class T1> static inline double unaligned_inner_product(const T1* a, const T1* b, size_t count) {
	    return inner_product<T1>(a,b,count);
	}
	
	static inline double aligned_inner_product(const double* a, const double* b, unsigned int count)
	{
	    if (is_aligned<16>(b))
		return double_inner_product<true>((const __m128d*) a, (const __m128d*) b, count>>1);
	    else
		return double_inner_product<false>((const __m128d*) a, (const __m128d*) b, count>>1);
	}

	template <class T1> static inline double unaligned_ssd(const T1* a, const T1* b, size_t count) {
	    return sum_squared_differences<T1>(a,b,count);
	}
	
	static inline long long unaligned_ssd(const byte* a, const byte* b, size_t count) {
	    return SumSquaredDifferences<long long, int, byte>::sum_squared_differences(a,b,count);
	}

	static inline double aligned_ssd(const double* a, const double* b, size_t count) 
	{
	    if (is_aligned<16>(b))
		return double_sum_squared_differences<true>((const __m128d*)a, (const __m128d*)b, count>>1);
	    else
		return double_sum_squared_differences<false>((const __m128d*)a, (const __m128d*)b, count>>1);
	}

	static inline long long aligned_ssd(const byte* a, const byte* b, size_t count) 
	{
	    if (is_aligned<16>(b)) 
		return byte_sum_squared_differences<true>((const __m128i*)a, (const __m128i*)b, count>>4);
	    else
		return byte_sum_squared_differences<false>((const __m128i*)a, (const __m128i*)b, count>>4);
	}	
    };

    void differences(const int32_t* a, const int32_t* b, int32_t* diff, unsigned int size)
    {
	maybe_aligned_differences<SSE2_funcs, int32_t, int32_t, 16, 4>(a,b,diff,size);
    }

    void differences(const double* a, const double* b, double* diff, unsigned int size)
    {
	maybe_aligned_differences<SSE2_funcs, double, double, 16, 2>(a,b,diff,size);
    }

    void add_multiple_of_sum(const double* a, const double* b, const double& c,  double* out, unsigned int count)
    {
	maybe_aligned_add_mul_add<SSE2_funcs, double, double, 16, 2>(a,b,c,out,count);
    }

    void assign_multiple(const double* a, const double& c,  double* out, unsigned int count)
    {
	maybe_aligned_assign_mul<SSE2_funcs, double, double, 16, 2>(a,c,out,count);
    }

    double inner_product(const double* a, const double* b, unsigned int count)
    {
	return maybe_aligned_inner_product<SSE2_funcs, double, double, 16, 2>(a,b,count);
    }

    double sum_squared_differences(const double* a, const double* b, size_t count)
    {
	return maybe_aligned_ssd<SSE2_funcs, double, double, 16, 2>(a,b,count);
    }

    long long sum_squared_differences(const byte* a, const byte* b, size_t count)
    {
	return maybe_aligned_ssd<SSE2_funcs, long long, byte, 16, 16>(a,b,count); 
    }
#endif
    
}
