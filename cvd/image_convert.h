#ifndef CVD_IMAGE_CONVERT_H
#define CVD_IMAGE_CONVERT_H

#include <cvd/internal/convert_pixel_types.h>
#include <cvd/internal/rgb_components.h>
#include <cvd/image.h>

namespace CVD
{
	namespace Pixel
	{
		extern WeightedRGB<> CIE;
		extern WeightedRGB<> red_only;
		extern WeightedRGB<> green_only;
		extern WeightedRGB<> blue_only;
		extern WeightedRGB<> uniform;
	}

	template<class D, class C, class Conv> Image<D> convert_image(const BasicImage<C>& from, Conv& cv)
	{
		Image<D> to(from.size());

		const C *fp, *fpe;
		D* tp;

		fp = from.data();
		fpe = fp + from.totalsize();

		tp = to.data();

		
		for(; fp < fpe; fp++, tp++)
			cv.convert_pixel(*fp, *tp);

		return to;
	}

	template<class D, class C> Image<D> convert_image(const BasicImage<C>& from)
	{
		return convert_image<D>(from, Pixel::CIE);
	}


}

#endif
