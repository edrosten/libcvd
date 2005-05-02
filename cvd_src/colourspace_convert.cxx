#include "cvd/image_convert_fwd.h"
#include "cvd/colourspaces.h"
#include "cvd/colourspace.h"
#include "cvd/byte.h"
#include "cvd/rgb.h"



namespace CVD
{
	
	template<> Image<Rgb<byte> > convert_image(const BasicImage<bayer>& from)
	{
		Image<Rgb<byte> > ret(from.size());

		ColourSpace::bayer_to_rgb(reinterpret_cast<const unsigned char*>(from.data()), 
								  reinterpret_cast<unsigned char*>(ret.data()), 
								  from.size().x, from.size().y);

		return ret;
	}

	template<> Image<byte> convert_image(const BasicImage<bayer>& from)
	{
		Image<byte> ret(from.size());

		ColourSpace::bayer_to_grey(reinterpret_cast<const unsigned char*>(from.data()), 
								  reinterpret_cast<unsigned char*>(ret.data()), 
								  from.size().x, from.size().y);

		return ret;
	}
	
	template<> Image<Rgb<byte> > convert_image(const BasicImage<yuv411>& from)
	{
		Image<Rgb<byte> > ret(from.size());

		ColourSpace::yuv411_to_rgb(reinterpret_cast<const unsigned char*>(from.data()),
								 from.totalsize(),
		                         reinterpret_cast<unsigned char*>(ret.data()));

		return ret;
	}

	template<> Image<byte> convert_image(const BasicImage<yuv411>& from)
	{
		Image<byte> ret(from.size());

		ColourSpace::yuv411_to_y(reinterpret_cast<const unsigned char*>(from.data()),
								 from.totalsize(),
		                         reinterpret_cast<unsigned char*>(ret.data()));

		return ret;
	}

}
