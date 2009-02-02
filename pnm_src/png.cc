#include "cvd/internal/io/png.h"
#include "cvd/image_io.h"
#include "cvd/config.h"

#include <png.h>
#include <cstdlib>

using namespace CVD;
using namespace CVD::Exceptions;
using namespace CVD::Exceptions::Image_IO;
using namespace PNG;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
// C++ istreams based I/O functions
// 
static void error_fn(png_structp png_ptr, png_const_charp error_msg)
{
	*(string*)(png_ptr->error_ptr) = error_msg;
}

static void warn_fn(png_structp, png_const_charp )
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

////////////////////////////////////////////////////////////////////////////////
//
// PNG reading functions
// 

string png_reader::datatype()
{
	return type;
}

string png_reader::name()
{
	return "PNG";
}

ImageRef png_reader::size()
{
	return my_size;
}


//Mechanically generate the pixel reading calls.
#define GEN1(X) void png_reader::get_raw_pixel_line(X*d){read_pixels(d);}
#define GEN3(X) GEN1(X) GEN1(Rgb<X>) GEN1(Rgba<X>)
GEN1(bool)
GEN3(unsigned char)
GEN3(unsigned short)



template<class P> void png_reader::read_pixels(P* data)
{
	if(datatype() != PNM::type_name<P>::name())
		throw ReadTypeMismatch(datatype(), PNM::type_name<P>::name());

	if(row  > (unsigned long)my_size.y)
		throw InternalLibraryError("CVD", "Read past end of image.");


	if(setjmp(png_jmpbuf(png_ptr)))     
		throw Exceptions::Image_IO::MalformedImage(error_string);
	
	unsigned char* cptr = reinterpret_cast<unsigned char*>(data);
	unsigned char** row_ptr = &cptr;

	png_read_rows(png_ptr, row_ptr, NULL, 1);
}



png_reader::png_reader(std::istream& in)
:i(in),type(""),row(0),png_ptr(0),info_ptr(0),end_info(0)
{
	//Read the header and make sure it really is a PNG...
	unsigned char header[8];

	in.read((char*)header, 8);

	if(png_sig_cmp(header, 0, 8))
		throw Exceptions::Image_IO::MalformedImage("Not a PNG image");

	
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
	int colour, interlace, dummy, depth;

	png_get_IHDR(png_ptr, info_ptr, &w, &h, &depth, &colour, &interlace, &dummy, &dummy);
	
	my_size.x = w;
	my_size.y = h;

	//Figure out the type name, and what processing to to.
	if(depth == 1)
	{	
		//Unpack bools to bytes to ease loading.
		png_set_packing(png_ptr);
		type = PNM::type_name<bool>::name();
	}
	else if(depth <= 8)
	{
		//Expand nonbool colour depths up to 8bpp
		if(depth < 8)
			png_set_gray_1_2_4_to_8(png_ptr);

		type = PNM::type_name<unsigned char>::name();
	}
	else
		type = PNM::type_name<unsigned short>::name();

	
	if(colour & PNG_COLOR_MASK_COLOR)
		if(colour & PNG_COLOR_MASK_ALPHA)
			type = "CVD::Rgba<" + type + ">";
		else
			type = "CVD::Rgb<" + type + ">";
	else
		if(colour & PNG_COLOR_MASK_ALPHA)
			type = "CVD::GreyAlpha<" + type + ">";
		else
			type = type;
	
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

png_reader::~png_reader()
{
	//Clear the stream of any remaining PNG bits.
	//It doesn't matter if there's an error here
	if(!setjmp(png_jmpbuf(png_ptr)))
		png_read_end(png_ptr, end_info);

	//Destroy all PNG structs
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
}


////////////////////////////////////////////////////////////////////////////////
//
// PNG writing functions.
//

png_writer::png_writer(ostream& out, ImageRef sz, const string& type_)
:row(0),o(out),size(sz),type(type_)
{
	//Create required structs
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &error_string, error_fn, warn_fn);
	if(!png_ptr)
		throw Exceptions::OutOfMemory();

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)     
	{        
		png_destroy_write_struct(&png_ptr,NULL);
		throw Exceptions::OutOfMemory();
	}

	//Set up error handling
	if(setjmp(png_jmpbuf(png_ptr)))     
	{         
		png_destroy_write_struct(&png_ptr, &info_ptr);
		throw Exceptions::Image_IO::WriteError(error_string);
	}

	//Set up stream IO
	png_set_write_fn(png_ptr, &o, write_fn, flush_fn);

	int c_type=0;
	int depth=0;

	if(type == "bool")
	{
		c_type = PNG_COLOR_TYPE_GRAY;
		depth=1;
	}
	else if(type == "unsigned char")
	{
		c_type = PNG_COLOR_TYPE_GRAY;
		depth=8;
	}
	else if(type == "unsigned short")
	{
		c_type = PNG_COLOR_TYPE_GRAY;
		depth=16;
	}
	else if(type == "CVD::Rgb<unsigned char>")
	{
		c_type = PNG_COLOR_TYPE_RGB;
		depth=8;
	}
	else if(type == "CVD::Rgb8")
	{
		c_type = PNG_COLOR_TYPE_RGB;
		depth=8;
		//Note the existence of meaningless filler.
		png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);
	}
	else if(type == "CVD::Rgb<unsigned short>")
	{
		c_type = PNG_COLOR_TYPE_RGB;
		depth=16;
	}
	else if(type == "CVD::Rgba<unsigned char>")
	{
		c_type = PNG_COLOR_TYPE_RGB_ALPHA;
		depth = 8;
	}
	else if(type == "CVD::Rgba<unsigned short>")
	{
		c_type = PNG_COLOR_TYPE_RGB_ALPHA;
		depth = 16;
	}
	else
		throw UnsupportedImageSubType("TIFF", type);


	//Set up the image type
	png_set_IHDR(png_ptr, info_ptr, size.x, size.y, depth, c_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	//Write the header 
	png_write_info(png_ptr, info_ptr);
	
	//Write the transformations
	#ifdef CVD_ARCH_LITTLE_ENDIAN
		if (depth == 16)
			png_set_swap(png_ptr);
	#endif

	//Pack from C++ bools to packed PNG bools
	//This has to be done _after_ writing the info struct.
	if(type == "bool")
		png_set_packing(png_ptr);

}

//Mechanically generate the pixel writing calls.
#undef GEN1
#undef GEN3
#define GEN1(X) void png_writer::write_raw_pixel_line(const X*d){write_line(d);}
#define GEN3(X) GEN1(X) GEN1(Rgb<X>) GEN1(Rgba<X>)
GEN1(bool)
GEN1(Rgb8)
GEN3(unsigned char)
GEN3(unsigned short)


template<class P> void png_writer::write_line(const P* data)
{
	unsigned char* chardata = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data));

	//Do some type checking
	if(type != PNM::type_name<P>::name())
		throw WriteTypeMismatch(type, PNM::type_name<P>::name());


	//Set up error handling
	if(setjmp(png_jmpbuf(png_ptr)))     
		throw Exceptions::Image_IO::WriteError(error_string);

	//Do some sanity checking
	if(row > size.y)
		throw InternalLibraryError("CVD", "Write past end of image.");


	unsigned char** row_ptr =  & chardata;
	png_write_rows(png_ptr, row_ptr, 1);

	row++;
}

png_writer::~png_writer()
{
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
}
