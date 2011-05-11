#include "cvd/vision.h"
#include "cvd/config.h"

namespace CVD
{
	namespace Internal{
				
			extern "C" {
				void cvd_asm_halfsample_mmx(const unsigned char*, unsigned char*, int, int);
			}
	}

	void halfSample(const BasicImage<byte>& in, BasicImage<byte>& out)
	{   
		if( (in.size()/2) != out.size())
			throw Exceptions::Vision::IncompatibleImageSizes("halfSample");

		if (is_aligned<8>(in.data()) && is_aligned<8>(out.data()) && (in.size().x % 8 == 0))
			Internal::cvd_asm_halfsample_mmx(in.data(), out.data(), in.size().x, in.size().y);
		else
			halfSample<byte>(in, out);
	}
}
