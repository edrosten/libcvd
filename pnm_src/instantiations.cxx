#include <cvd/image_io.h>
#include <cvd/internal/pnm/convert_builtin_types.h>
#include <cvd/internal/pnm/cvd_rgb.h>
#include <cvd/internal/pnm/load_and_save.h>
#include <cvd/arch.h>

#include <iostream>
using namespace std;

namespace CVD
{
namespace PNM
{

	//This file contains instantiations of all the misc. things that need to be
	//instantiated. 

	namespace convert
	{
		//Some useful rgb weightings
		rgb_weights		uniform(1./3, 1./3, 1./3);
		rgb_weights		cie(0.299, 0.587, 0.114);
		rgb_weights		red_only(1, 0, 0);
		rgb_weights		green_only(0, 1, 0);
		rgb_weights		blue_only(0, 0, 1);


		rgb_weights::rgb_weights(float rr, float gg, float bb)
		:r(rr),g(gg),b(bb)
		{
		}
	}


	//These functions aren't templated anymore, so either they have to be inline, or they have to be instantiated.
	template<> bool fast_load(Image_IO::image_in& i, unsigned char* mem, unsigned long nlines, convert::weighted_rgb<unsigned char>)
	{
		//Outgoing memory is 1 byte per element, greyscale. Check incoming
		if(!i.is_2_byte() && !i.is_rgb())
		{
			i.get_raw_pixel_lines(mem, nlines);
			return true;
		}
		else
			return false;
	}

	template<> bool fast_load(Image_IO::image_in& i, unsigned short* mem, unsigned long nlines, convert::weighted_rgb<unsigned short>)
	{	
		//Outgoing is 2 byte greyscale. Check incoming image
		if(i.is_2_byte() && !i.is_rgb())
		{
			i.get_raw_pixel_lines(mem, nlines);
			return true;
		}
		else
			return false;
	}

	template<> bool fast_save(Image_IO::image_out& i, const unsigned char* mem, unsigned long nlines, convert::weighted_rgb<unsigned char>)
	{
		if(!i.is_2_byte() && !i.is_rgb())
		{
			i.write_raw_pixel_lines(mem, nlines);
			return true;
		}
		else
			return false;
	}

	template<> bool fast_save(Image_IO::image_out& i, const unsigned short* mem, unsigned long nlines, convert::weighted_rgb<unsigned short>)
	{
		if(i.is_2_byte() && !i.is_rgb())
		{
			i.write_raw_pixel_lines(mem, nlines);
			return true;
		}
		else
			return false;
	}
	

	//Can we treat CVD::RGB<x> as an array of x with no padding?
	#if defined PACKED_RGB
	template<> bool fast_load(Image_IO::image_in& i, CVD::Rgb<unsigned char>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned char> >)
	{
		if(!i.is_2_byte() && i.is_rgb())
		{
			i.get_raw_pixel_lines((unsigned char*)mem, nlines);
			return true;
		}
		else
			return false;
	}

	template<> bool fast_save(Image_IO::image_out& i, const CVD::Rgb<unsigned char>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned char> >)
	{
		if(!i.is_2_byte() && i.is_rgb())
		{
			i.write_raw_pixel_lines((unsigned char*)mem, nlines);
			return true;
		}
		else
			return false;
	}

	template<> bool fast_load(Image_IO::image_in& i, CVD::Rgb<unsigned short>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned short> >)
	{
		if(i.is_2_byte() && i.is_rgb())
		{
			i.get_raw_pixel_lines((unsigned short*)mem, nlines);
			return true;
		}
		else
			return false;
	}

	template<> bool fast_save(Image_IO::image_out& i, const CVD::Rgb<unsigned short>* mem, unsigned long nlines, convert::weighted_rgb<CVD::Rgb<unsigned short> >)
	{
		if(i.is_2_byte() && i.is_rgb())
		{
			i.write_raw_pixel_lines((unsigned short*)mem, nlines);
			return true;
		}
		else
			return false;
	}
	#endif

}
}
