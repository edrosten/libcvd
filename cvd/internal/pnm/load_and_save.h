#ifndef PNM_LOAD_RAW_H
#define PNM_LOAD_RAW_H

#include <memory>

#include <cvd/internal/disk_image.h>
namespace CVD
{
namespace PNM
{

////////////////////////////////////////////////////////////////////////////////
//
// Get a bunch of image lines in to an image of a certain type
// Perform type conversion of necessary
//
// Load the image directly in to memory if possible (this is done with a
// mixture of compile and load time checking). Its endian-agnostic since that
// conversion is minimal and is performed in-place by the pnm_in routines
//
// If not, then load the image in to a buffer and use the provided conversion
// routines to turn it in to the right type.
//
// The canonical conversion class is pix_conv defined in pnm_load_builtin.h, so
// see that for the interface required.
//
////////////////////////////////////////////////////////////////////////////////



//Fast load template functions. If the pnm can be fast loaded, then fast_load
//will load the pnm and return true, otherwise it will return false. All errors
//during loading are signalled by exceptions.

//These functions can be further specialised by an addon module to allow fast
//loading of PNMs in to fancier (eg RGB) types when the type is packed 
//correctly, for instance, or for PGMs of the correct types.

template<class C, class D> inline bool fast_load(Image_IO::image_in& i, C* mem, unsigned long nlines, D)
{
	//In general, PNMs can not be fast loaded.
	//specializations provided elsewhere. These will determine if fast loading is
	//possible based on the destination data type, the input image type and the 
	//input image depth.
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// slow_load: this is the generic loader that should always work. It will load
//            the image and convert the pixels to the correct type.

template<class ld, class im, class conv> void slow_load(Image_IO::image_in& i, ld* mem, unsigned long nl, conv c)
{

	//Allocate an exception-safe buffer for reading in the image.
	im* buf = new im[nl * i.elements_per_line()];
	std::auto_ptr<im> m_buff(buf);

	if(buf == NULL)
		throw CVD::Exceptions::OutOfMemory();
	
	//Read in the image
	i.get_raw_pixel_lines(buf, nl);
	
	unsigned long j, max;

	max = nl * i.x_size();
	
	//Cook the pixels:
	if(i.is_rgb())
		for(j=0 ; j < max; j++, buf+=3)
			c.conv_from_rgb(*mem++, buf);
	else
		for(j=0 ; j < max; j++)
			c.conv_from_lum(*mem++, buf++);
}

////////////////////////////////////////////////////////////////////////////////
//
// get_lines: wrapper function for the fast_load and slow_load functions. This
// 			  autodetects the types at run and compile time and calls the 
//			  correct loading code.
//
//			  PNMs can be either RGB or greyscale. The image can be of an type,
//			  so a way of converting from RGB and luminance (for loading) and
//			  to RGB/luminance is required. This is provided by the `conv'
//			  class.
//			  See load_builtin_types.h for details about the conv class


template<class C, class conv> void get_lines(Image_IO::image_in& i, C* mem, unsigned long nlines, conv c)
{
	//Fast load if possible	
	if(fast_load(i, mem, nlines, c))
		return;

	//If its not compatible, it needs to be loaded in to temporary memory and
	//the converted to the right type;
	if(i.is_2_byte())	
		slow_load<C, unsigned short, conv>(i, mem, nlines, c);
	else
		slow_load<C, unsigned char, conv>(i, mem, nlines, c);
}


template<class C, class conv> void load_image(Image_IO::image_in& i, C* mem, conv c)
{
	//FIXME: for slow load, load and convert in chunks to reduce memory usage
	get_lines(i, mem, i.y_size(), c);
}


////////////////////////////////////////////////////////////////////////////////
//
//Saving logic is very similar, so see above for documentation.
//


template<class C, class D> inline bool fast_save(Image_IO::image_out& i, const C* mem, unsigned long nlines, D)
{
	return false;
}

template<class sv, class im, class conv> void slow_save(Image_IO::image_out& o, const sv* mem, unsigned long nl, conv c)
{

	//Allocate an exception-safe buffer for converting the image
	im* bp, *buf = new im[nl * o.elements_per_line()];
	std::auto_ptr<im> m_buff(buf);

	if(buf == NULL)
		throw CVD::Exceptions::OutOfMemory();
	
	unsigned long j, max;

	max = nl * o.x_size();
	
	bp = buf;
	
	//Cook the pixels:
	if(o.is_rgb())
		for(j=0 ; j < max; j++, bp+=3)
			c.conv_to_rgb(*mem++, bp);
	else
		for(j=0 ; j < max; j++)
			c.conv_to_lum(*mem++, bp++);
	
	//Write in the image
	o.write_raw_pixel_lines(buf, nl);
}

template<class C, class conv> void save_lines(Image_IO::image_out& i, const C* mem, unsigned long nlines, conv c)
{
	//Fast load if possible	
	if(fast_save(i, mem, nlines, c))
		return;

	//If its not compatible, it needs to be converted in to temporary memory and
	//then written
	if(i.is_2_byte())	
		slow_save<C, unsigned short, conv>(i, mem, nlines, c);
	else
		slow_save<C, unsigned char, conv>(i, mem, nlines, c);
}

template<class C, class conv> void save_image(Image_IO::image_out& i, const C* mem, conv c)
{
	//FIXME: for slow save, save and convert in chunks to reduce memory usage
	save_lines(i, mem, i.y_size(), c);
}

}
}
#endif
