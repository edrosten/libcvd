#include "cvd/image_io.h"
#include "cvd/internal/disk_image.h"
#include "cvd/internal/avaliable_images.hh"
#include "pnm_src/pnm_grok.h"

#ifdef CVD_IMAGE_HAS_JPEG
	#include "pnm_src/jpeg.h"
#endif

#include <sstream>

using namespace CVD;
using namespace std;

Exceptions::Image_IO::ImageSizeMismatch::ImageSizeMismatch(const ImageRef& src, const ImageRef& dest)
{
	ostringstream o;
	o << 
"Image load: Size mismatch when loading an image (size " << src << ") in to a non\
resizable image (size " << dest << ").";

	what = o.str();
}

Exceptions::Image_IO::MalformedImage::MalformedImage(const string& why)
{
	what = "Image input: " + why;
}

Exceptions::Image_IO::UnsupportedImageType::UnsupportedImageType()
{
	what = "Image input: Unsuppported image type.";
}

Exceptions::Image_IO::EofBeforeImage::EofBeforeImage()
{
	what = "End of file occured before image.";
}

Exceptions::Image_IO::WriteError::WriteError(const string& s)
{
	what = "Error writing " + s;
}
namespace CVD
{
namespace Image_IO
{
/*
long image_base::elements_per_line() const
{
	if(m_is_rgb)
		return xs * 3;
	else
		return xs;
}

long image_base::x_size() const
{
	return xs;
}

long image_base::y_size() const
{
	return ys;
}

bool image_base::is_2_byte() const
{
	return m_is_2_byte;
}

bool image_base::is_rgb() const
{
	return m_is_rgb;
}
*/
image_in::~image_in(){}
image_out::~image_out(){}


image_out* image_factory::out(std::ostream& o, long xsize, long ysize, ImageType::ImageType t, bool try_rgb, bool try_2byte, const std::string& c)
{
	switch(t)
	{
		case ImageType::PNM:
			return new CVD::PNM::pnm_out(o, xsize, ysize, try_rgb, try_2byte, c);
			break;

		#ifdef CVD_IMAGE_HAS_JPEG
			case ImageType::JPEG:
				return new CVD::PNM::jpeg_out(o, xsize, ysize, try_rgb, try_2byte, c);
				break;
		#endif

		default:
			throw Exceptions::Image_IO::UnsupportedImageType();
			return NULL;
	}
}

image_in* image_factory::in(std::istream& i)
{
	unsigned char c = i.get();

	if(i.eof())
		throw Exceptions::Image_IO::EofBeforeImage();

	i.putback(c);

	if(c == 'P')
		return new CVD::PNM::pnm_in(i);
	#ifdef CVD_IMAGE_HAS_JPEG
		else if(c == 0xff)
			return new CVD::PNM::jpeg_in(i);
	#endif
	else
		throw Exceptions::Image_IO::UnsupportedImageType();

}


}
}



