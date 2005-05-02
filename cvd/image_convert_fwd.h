#ifndef CVD_IMAGE_CONVERT_FWD_H
#define CVD_IMAGE_CONVERT_FWD_H

#include <cvd/image.h>

namespace CVD
{
	//Forward declarations for convert_image
	template<class D, class C, class Conv> 	Image<D> convert_image(const BasicImage<C>& from, Conv& cv);
	template<class D, class C> Image<D> convert_image(const BasicImage<C>& from);
}

#endif
