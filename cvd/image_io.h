#ifndef CVD_IMAGE_IO_H
#define CVD_IMAGE_IO_H

#include <cvd/exceptions.h>
#include <cvd/image.h>

namespace CVD
{
	namespace Exceptions
	{
		namespace Image_IO
		{
			struct All: public CVD::Exceptions::All
			{};

			struct UnsupportedImageType: public All
			{
				UnsupportedImageType();
			};

			struct EofBeforeImage: public All
			{
				EofBeforeImage();
			};

			struct MalformedImage: public All
			{
				MalformedImage(const std::string &);
			};

			struct ImageSizeMismatch: public All
			{
				ImageSizeMismatch(const ImageRef& src, const ImageRef& dest);
			};


		}
	}
}
#include <cvd/internal/pnm/pnm_cvd_image.h>
#include <cvd/internal/pnm/cvd_rgb.h>



#endif
