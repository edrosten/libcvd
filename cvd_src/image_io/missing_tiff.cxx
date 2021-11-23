#include "cvd/internal/io/tiff.h"

using namespace CVD;

class CVD::TIFF::TIFFPimpl
{
};

namespace{
[[noreturn]] void error(){
	throw Exceptions::Image_IO::MissingImageType("TIFF");	
}
}

tiff_reader::tiff_reader(std::istream& i)
{
	error();
}


tiff_reader::~tiff_reader()
{} 

std::string tiff_reader::datatype()
{
	error();
}

std::string tiff_reader::name()
{
	error();
}

bool tiff_reader::top_row_first()
{
	error();
};

ImageRef tiff_reader::size()
{
	error();
};

//Mechanically generate the pixel reading calls.
#define GEN1(X) void tiff_reader::get_raw_pixel_line(X*){error();}
#define GEN3(X) GEN1(X) GEN1(Rgb<X>) GEN1(Rgba<X>)

GEN1(bool)
GEN3(unsigned char)
GEN3(unsigned short)
GEN3(float)
GEN3(double)



class CVD::TIFF::TIFFWritePimpl
{
};

tiff_writer::tiff_writer(std::ostream&, ImageRef, const std::string&, const std::map<std::string, Parameter<>>&)
{
	error();
}

tiff_writer::~tiff_writer()
{
}

#undef GEN1
//Mechanically generate the pixel reading calls.
#define GEN1(X) \
	void tiff_writer::write_raw_pixel_line(const X*) {error();}

GEN1(bool)
GEN3(unsigned char)
GEN3(unsigned short)
GEN3(float)
GEN3(double)

