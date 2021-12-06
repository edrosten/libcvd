#include "cvd/internal/io/png.h"

using namespace CVD;

class CVD::PNG::PNGPimpl
{
};

namespace
{
[[noreturn]] void error()
{
	throw Exceptions::Image_IO::MissingImageType("TIFF");
}
}

CVD::PNG::Reader::Reader(std::istream&)
{
	error();
}

CVD::PNG::Reader::~Reader()
{
}

std::string CVD::PNG::Reader::datatype()
{
	error();
}

std::string CVD::PNG::Reader::name()
{
	error();
}

bool CVD::PNG::Reader::top_row_first()
{
	error();
};

ImageRef CVD::PNG::Reader::size()
{
	error();
};

#define GEN1(X) \
	void CVD::PNG::Reader::get_raw_pixel_line(X* d) { error(); }
#define GEN3(X)  \
	GEN1(X)      \
	GEN1(Rgb<X>) \
	GEN1(Rgba<X>)
GEN1(bool)
GEN3(unsigned char)
GEN3(unsigned short)

class CVD::PNG::WriterPimpl
{
};

CVD::PNG::png_writer::png_writer(std::ostream&, ImageRef, const std::string&, const std::map<std::string, Parameter<>>&)
{
	error();
}

CVD::PNG::png_writer::~png_writer()
{
}
#undef GEN1
#define GEN1(X) \
	void CVD::PNG::png_writer::write_raw_pixel_line(const X* d) { error(); }
GEN1(bool);
GEN1(Rgb8)
GEN3(unsigned char)
GEN3(unsigned short)
