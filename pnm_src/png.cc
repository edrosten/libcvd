#include "cvd/internal/io/png.h"
#include "cvd/image_io.h"
#include "cvd/config.h"

#include <png.h>

using namespace CVD;
using namespace PNG;
using namespace std;


static void error_fn(png_structp png_ptr, png_const_charp error_msg)
{
	*(string*)(png_ptr->error_ptr) = error_msg;
}

static void warn_fn(png_structp png_ptr, png_const_charp error_msg)
{
}

static void read_fn(png_structp png_ptr, unsigned char*  data, size_t numbytes)
{
	istream* i = (istream*)png_get_io_ptr(png_ptr);
	i->read((char*) data, numbytes);

	//What to do on stream failure?
	//There is no return value, and I do not know if longjmp is safe here.
	//Anyway, multiple rereads of the data will cause the PNG read
	//to fail because it has internal checksums
}



/*
static void write_fn(png_structp png_ptr, unsigned char*  data, size_t numbytes)
{
	ostream* o = (ostream*)png_get_io_ptr(png_ptr);
	o->write((char*) data, numbytes);
}

static void flush_fn(png_structp png_ptr)
{
	ostream* o = (ostream*)png_get_io_ptr(png_ptr);
	(*o) << flush;
}
*/

void png_in::unpack_to_bytes()
{
	//No error checking. Only call for Grey images
	png_set_packing(png_ptr);
}

void png_in::expand_depth_to_8()
{
	//No error checking. Only call for Grey images
	png_set_gray_1_2_4_to_8(png_ptr);
}

void png_in::get_raw_pixel_lines(unsigned char** row_ptr, int nlines)
{
	if(depth == 16)
		throw Exceptions::Image_IO::ReadTypeMismatch(1);

	if(setjmp(png_jmpbuf(png_ptr)))     
		throw Exceptions::Image_IO::MalformedImage(error_string);

	//NB no check for read past end.
	png_read_rows(png_ptr, row_ptr, NULL, nlines);
}


void png_in::get_raw_pixel_lines(unsigned short** row_ptr, int nlines)
{
	if(depth != 16)
		throw Exceptions::Image_IO::ReadTypeMismatch(0);

	if(setjmp(png_jmpbuf(png_ptr)))     
		throw Exceptions::Image_IO::MalformedImage(error_string);

	//NB no check for read past end.
	png_read_rows(png_ptr, (unsigned char**)row_ptr, NULL, nlines);
}

png_in::png_in(std::istream& in)
:i(in)
{
	unsigned char header[8];
	fread(header, 1, 8, stdin);

	if(png_sig_cmp(header, 0, 8))
	{
		throw Exceptions::Image_IO::MalformedImage("Not a PNG image");
		exit(1);
	}


	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);


	if(!png_ptr)
		throw Exceptions::OutOfMemory();
	
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		throw Exceptions::OutOfMemory();
	}

	end_info = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		throw Exceptions::OutOfMemory();
	}

	if(setjmp(png_jmpbuf(png_ptr)))     
	{         
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		throw Exceptions::Image_IO::MalformedImage(error_string);
	}

	png_set_error_fn(png_ptr, &error_string, error_fn, warn_fn);

	//png_init_io(png_ptr, stdin);

	png_set_read_fn(png_ptr, &i, read_fn);


	png_set_sig_bytes(png_ptr, 8);
	
	png_read_info(png_ptr, info_ptr);

	png_uint_32 w, h;
	int colour, interlace, dummy;

	png_get_IHDR(png_ptr, info_ptr, &w, &h, &depth, &colour, &interlace, &dummy, &dummy);

	width = w;
	height = h;
	
	if(colour & PNG_COLOR_MASK_COLOR)
		if(colour & PNG_COLOR_MASK_ALPHA)
			colour_type = RgbAlpha;
		else
			colour_type = Rgb;
	else
		if(colour & PNG_COLOR_MASK_ALPHA)
			colour_type = GreyAlpha;
		else
			colour_type = Grey;
	
	//Get rid of palette, by transforming it to RGB
	if(colour == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	if(interlace != PNG_INTERLACE_NONE)
		throw Exceptions::Image_IO::UnsupportedImageSubType("PNG", "Interlace not yet supported");

	#ifdef CVD_ARCH_LITTLE_ENDIAN
		if(depth == 16)
			  png_set_swap(png_ptr);
	#endif
}

void png_in::read_end()
{
	//It doesn't matter if there's an error here
	if(!setjmp(png_jmpbuf(png_ptr)))
		png_read_end(png_ptr, end_info);
}

png_in::~png_in()
{
	//Destroy all PNG structs
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
}


