#include <cvd/config.h>
#include <cvd/image_convert.h>
#include <cvd/utility.h>
namespace CVD {

	void ConvertImage<Rgb<byte>, byte, Pixel::CIE<Rgb<byte>, byte>, 1>::convert(const SubImage<Rgb<byte> >& from, SubImage<byte>& to) 
	{
		const Rgb<byte>* rgb = from.data();
		byte* gray = to.data(), *gend = to.data() + to.totalsize();

		while (gray < gend)
		{
			Pixel::CIE<Rgb<byte>,byte>::convert(*rgb, *gray);
			rgb++;
			gray++;
		}
	}
}
