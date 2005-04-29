#include "cvd/image_convert_fwd.h"
#include "cvd/colourspaces.h"
#include "cvd/byte.h"
#include "cvd/rgb.h"



namespace CVD
{
		
	/// Convert Bayer pattern of the form ??? to greyscale data
	/// @param bggr The input data
	/// @param grey The output data
	/// @param width The width of the image
	/// @param height The height of the image
	/// @ingroup gImageIO
	template<> Image<byte> convert_image(const BasicImage<bayer>& from);	
	
	/// Convert Bayer pattern of the form ??? to rgb444 data
	/// @param bggr The input data
	/// @param grey The output data
	/// @param width The width of the image
	/// @param height The height of the image
	/// @ingroup gImageIO
	template<> Image<Rgb<byte> > convert_image(const BasicImage<bayer>& from);
	
	
	/// Convert YUV 411 pixel data to RGB
	/// @param yuv411 The input data
	/// @param npix The number of pixels
	/// @param out The output data
	/// @ingroup gImageIO
	template<> Image<byte> convert_image(const BasicImage<yuv411>& from);


	/// Convert YUV 411 pixel data to Y only
	/// @param yuv411 The input data
	/// @param npix The number of pixels
	/// @param out The output data
	/// @ingroup gImageIO
	template<> Image<byte> convert_image(const BasicImage<yuv411>& from);
}
