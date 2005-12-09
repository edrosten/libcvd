#include <cvd/config.h>
#include <cvd/image_convert.h>
#include <cvd/utility.h>
#include <cvd/internal/assembly.h>
namespace CVD {

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
    void ConvertImage<Rgb<byte>, byte, Pixel::CIE<Rgb<byte>, byte>, 1>::convert(const BasicImage<Rgb<byte> >& from, BasicImage<byte>& to) {
	const Rgb<byte>* rgb = from.data();
	byte* gray = to.data();
	int count=0;
	while (!is_aligned<16>(rgb) || !is_aligned<16>(gray)) {
	    Pixel::CIE<Rgb<byte>,byte>::convert(*rgb, *gray);
	    rgb++;
	    gray++;
	    count++;
	  }
	if (count < from.totalsize())
	    Internal::Assembly::rgb_to_gray((const byte*)rgb, gray, from.totalsize()-count, 77, 150, 29);
    }
#endif
}
