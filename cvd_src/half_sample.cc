#include "cvd/vision.h"
#include "cvd/config.h"

#ifdef CVD_INTERNAL_HAVE_HALFSAMPLE_SSE16
#include <emmintrin.h>
#endif


namespace CVD
{
	namespace Internal{
				
		#if CVD_INTERNAL_HAVE_HALFSAMPLE_MMX8
			extern "C" {
				void cvd_asm_halfsample_mmx(const unsigned char*, unsigned char*, int, int);
			}
		#endif

		#ifdef CVD_INTERNAL_HAVE_HALFSAMPLE_SSE16
		void halfSampleSSE2(const byte* in, byte* out, int w, int h) 
		{
			const unsigned long long mask[2] = {0x00FF00FF00FF00FFull, 0x00FF00FF00FF00FFull};
			//byte test[16];
			const byte* nextRow = in + w;
			__m128i m = _mm_loadu_si128((const __m128i*)mask);
			int sw = w >> 4;
			int sh = h >> 1;

			for (int i=0; i<sh; i++) 
			{
				for (int j=0; j<sw; j++) 
				{
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


	}

	void halfSample(const BasicImage<byte>& in, BasicImage<byte>& out)
	{   
		if( (in.size()/2) != out.size())
			throw Exceptions::Vision::IncompatibleImageSizes("halfSample");

		#if CVD_INTERNAL_HAVE_HALFSAMPLE_SSE16
			if (is_aligned<16>(in.data()) && is_aligned<16>(out.data()) && ((in.size().x % 16) == 0))
			{
				Internal::halfSampleSSE2(in.data(), out.data(), in.size().x, in.size().y);
				return;
			}
		#endif

		#ifdef CVD_INTERNAL_HAVE_HALFSAMPLE_MMX8
		if (is_aligned<8>(in.data()) && is_aligned<8>(out.data()) && (in.size().x % 8 == 0))
		{
			Internal::cvd_asm_halfsample_mmx(in.data(), out.data(), in.size().x, in.size().y);
			return;
		}

		#endif

		halfSample<byte>(in, out);
	}
}
