/*******************************************************************************
	convert_builtin_types.h

This file contains classes that allow images consisting of any of the builtin
types to be converted to/from PNM images. Ie, luminance images of floats, 
ints, chars, etc can be loaded in to / saved from. 

This file also documents the interface that functions must have in order to 
convert to/from PNM images from/to user defined types. Classes must define the
following functions (whether by overloading or by templating) if they want to be
used with images of type foo:

The reason for the pass by reference is that something (C++ or g++) won't
partially specialize on the return type.

class convert
{
	public:
		//Required for loading
		
		void conv_from_rgb(foo& pixel, const unsigned char* rgb_in);
		void conv_from_rgb(foo& pixel, const unsigned short* rgb_in);

		void conv_from_lum(foo& pixel, const unsigned char* lum_in);
		void conv_from_lum(foo& pixel, const unsigned short* lum_in);


		//Required for saving

		void conv_to_rgb(const foo& pixel, unsigned char* rgb_out);
		void conv_to_rgb(const foo& pixel, unsigned short* rgb_out);

		void conv_to_lum(const foo& pixel, unsigned char* lum_out);
		void conv_to_lum(const foo& pixel, unsigned short* lum_out);

};

where for rgb, the memory contains RGBRGBRGB...

The classes here use the luminance transcribing functions defined in
transcribe_luminance_types.h. To use different transcribing rules, classes
can be written to replace these, since all loading and saving is templated on
the conversion types. An example of a replacement class is given here which
performs user-specified RGB weighting when loading an RGB image.

If the classes were more complex, then they should probably be templated on the
transcribing function as well, so that different transcribing functions could
be painlessly substituted.

These types are meant to be specialized for non-luminance image types.

*******************************************************************************/

#ifndef PNM_CONVERT_BUILTIN_TYPES
#define PNM_CONVERT_BUILTIN_TYPES


#include <cvd/internal/disk_image.h>
#include <cvd/internal/pnm/load_and_save.h>
#include <cvd/internal/pnm/transcribe_luminance_types.h>


#include <cvd/internal/pnm/name_builtin_types.h>
namespace CVD
{
namespace PNM
{

////////////////////////////////////////////////////////////////////////////////
//
// Convert pixels from  PNM types to any builtin type
//

namespace convert
{
	struct rgb_weights
	{	
			rgb_weights(float rr, float gg, float bb);
			float r, g, b;
	};

	extern class rgb_weights cie;
	extern class rgb_weights red_only;
	extern class rgb_weights green_only;
	extern class rgb_weights blue_only;
	extern class rgb_weights uniform;



	#define PNM_WEIGHTED_RGB_CONSTRUCTORS									\
			weighted_rgb(float rr, float gg, float bb) : rgb_weights(rr,gg,bb){}\
			weighted_rgb(const rgb_weights& rgbw) : rgb_weights(rgbw) {}


	template<class O> class weighted_rgb: private rgb_weights
	{
		public:
			
			PNM_WEIGHTED_RGB_CONSTRUCTORS

			template<class I> inline void conv_from_rgb(O& opix, const I*rgb) const throw()
			{
				float p;

				p = (*rgb++) * r;
				p+= (*rgb++) * g;
				p+= (*rgb) * b;
				opix = pix_convert_lum<O>((I)p);
			}

			template<class I> inline void conv_from_lum(O& opix, const I* lum) const throw()
			{
				opix = pix_convert_lum<O>(*lum);
			}

			template<class I> inline void conv_to_lum(const O& lum, I* ilum) const throw()
			{
				*ilum = pix_convert_lum<I>(lum);
			}

			template<class I> inline void conv_to_rgb(const O& lum, I* irgb) const throw()
			{
				*irgb = pix_convert_lum<I>(lum);
				*(irgb+1) = *irgb;
				*(irgb+2) = *irgb;
			}
	};

}
	//These kinds of image can be fast-loaded, ie loaded without conversion. the pnm grokking
	//library sorts out how best to read or write them.

	template<> 
	bool fast_load(Image_IO::image_in& i, unsigned char* mem, unsigned long nlines, convert::weighted_rgb<unsigned char>);
	
	template<>
	bool fast_load(Image_IO::image_in& i, unsigned short* mem, unsigned long nlines, convert::weighted_rgb<unsigned short>);
	
	
	//And fast saved

	template<> 
	bool fast_save(Image_IO::image_out& i, const unsigned char* mem, unsigned long nlines, convert::weighted_rgb<unsigned char>);
	
	template<> 
	bool fast_save(Image_IO::image_out& i, const unsigned short* mem, unsigned long nlines, convert::weighted_rgb<unsigned short>);

}
}
#endif
