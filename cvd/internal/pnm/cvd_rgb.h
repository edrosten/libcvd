#ifndef PNM_LOAD_RGB_H
#define PNM_LOAD_RGB_H

#include <cvd/rgb.h>
#include <cvd/rgba.h>
#include <cvd/rgb8.h>

#include <cvd/arch.h>
#include <cvd/internal/disk_image.h>
#include <cvd/internal/pnm/convert_builtin_types.h>
#include <cvd/internal/pnm/name_CVD_rgb_types.h>

namespace CVD
{
namespace PNM
{

		template<class C> struct best_length<CVD::Rgb<C> > {static const int val = best_length<C>::val;};
		template<class C> struct best_length<CVD::Rgba<C> > {static const int val = best_length<C>::val;};
		template<> struct best_length<CVD::Rgb8> {static const int val = 0;};
namespace convert
{


//Specialize for the Rgb8 type

template<> class weighted_rgb<CVD::Rgb8>: private rgb_weights
{
	public:
		
		PNM_WEIGHTED_RGB_CONSTRUCTORS

		template<class I> inline void conv_from_rgb(CVD::Rgb8& op, I* rgb) const throw()
		{
			op.red = pix_convert_lum<unsigned char>(*rgb++);
			op.green = pix_convert_lum<unsigned char>(*rgb++);
			op.blue = pix_convert_lum<unsigned char>(*rgb);
			op.dummy = 0;
		}

		template<class I> inline void conv_to_rgb(const CVD::Rgb8& p, I* o) const throw()
		{
			*(o++) = pix_convert_lum<I>(p.red);
			*(o++) = pix_convert_lum<I>(p.green);
			*o = pix_convert_lum<I>(p.blue);
		}

		template<class I> inline void conv_from_lum(CVD::Rgb8& op, I* lum) const throw()
		{
			op.red = op.green = op.blue = pix_convert_lum<unsigned char>(*lum);
			op.dummy = 0;
		}


		template<class I> inline void conv_to_lum(const CVD::Rgb8& p, I*lum) const throw()
		{
			//Rgb8 has unsigned chare as its internam data type
			*lum = pix_convert_lum<I>((unsigned char)(p.red * r + p.green * g + p.blue * b));
		}
};



//Specialise pix_conv for Tom's CVD Rgb type
template<class O> class weighted_rgb<CVD::Rgb<O> >: private rgb_weights
{
	public:

		PNM_WEIGHTED_RGB_CONSTRUCTORS
	
		template<class I> inline void conv_to_lum(const CVD::Rgb<O>& p, I* o) const throw()
		{
			*o = pix_convert_lum<I>(static_cast<O>(r*p.red + g*p.green + b*p.blue));
		}

		template<class I> inline void conv_from_rgb(CVD::Rgb<O>& op, const I* rgb) const throw()
		{
			op.red = pix_convert_lum<O>(*rgb++);
			op.green = pix_convert_lum<O>(*rgb++);
			op.blue = pix_convert_lum<O>(*rgb);
		}

		template<class I> inline void conv_from_lum(CVD::Rgb<O>& op, const I* lum) const throw()
		{
			op.red = op.green = op.blue = pix_convert_lum<O>(*lum);
		}

		template<class I> inline void conv_to_rgb(const CVD::Rgb<O>& p, I* o) const throw()
		{
			*(o++) = pix_convert_lum<I>(p.red);
			*(o++) = pix_convert_lum<I>(p.green);
			*o = pix_convert_lum<I>(p.blue);
		}
};

template<class O> class weighted_rgb<CVD::Rgba<O> >: private rgb_weights
{
	public:

		PNM_WEIGHTED_RGB_CONSTRUCTORS
	
		template<class I> inline void conv_to_lum(const CVD::Rgba<O>& p, I* o) const throw()
		{
			*o = pix_convert_lum<I>(static_cast<O>(r*p.red + g*p.green + b*p.blue));
		}

		template<class I> inline void conv_from_rgb(CVD::Rgba<O>& op, const I* rgb) const throw()
		{
			op.red = pix_convert_lum<O>(*rgb++);
			op.green = pix_convert_lum<O>(*rgb++);
			op.blue = pix_convert_lum<O>(*rgb);
			op.alpha = pix_convert_lum<O>(0);
		}

		template<class I> inline void conv_from_lum(CVD::Rgba<O>& op, const I* lum) const throw()
		{
			op.red = op.green = op.blue = pix_convert_lum<O>(*lum);
		}

		template<class I> inline void conv_to_rgb(const CVD::Rgba<O>& p, I* o) const throw()
		{
			*(o++) = pix_convert_lum<I>(p.red);
			*(o++) = pix_convert_lum<I>(p.green);
			*o = pix_convert_lum<I>(p.blue);
		}
};
}

#ifdef PNM_PACKED_RGB 
	template<> bool fast_load(Image_IO::image_in& i, CVD::Rgb<unsigned char>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned char> >);
	template<> bool fast_save(Image_IO::image_out& i, const  CVD::Rgb<unsigned short>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned short> >);
	template<> bool fast_load(Image_IO::image_in& i, CVD::Rgb<unsigned char>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned char> >);
	template<> bool fast_save(Image_IO::image_out& i, const CVD::Rgb<unsigned short>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned short> >);
#endif






}

}

#endif
