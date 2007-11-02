/*                       
			 This file is part of the CVD Library.

			 Copyright (C) 2005 The Authors

			 This library is free software; you can redistribute it and/or
			 modify it under the terms of the GNU Lesser General Public
			 License as published by the Free Software Foundation; either
			 version 2.1 of the License, or (at your option) any later version.

			 This library is distributed in the hope that it will be useful,
			 but WITHOUT ANY WARRANTY; without even the implied warranty of
			 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
			 Lesser General Public License for more details.

			 You should have received a copy of the GNU Lesser General Public
			 License along with this library; if not, write to the Free Software
			 Foundation, Inc., 
			 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef CVD_INTERNAL_IO_PNG_H
#define CVD_INTERNAL_IO_PNG_H
#include <iostream>
#include <vector>
#include <string>

#include <cvd/image.h>
#include <cvd/internal/load_and_save.h>
#include <cvd/internal/convert_pixel_types.h>

struct png_struct_def;
struct png_info_struct;


namespace CVD{
namespace PNG{


struct png_in
{
	enum
	{
		Grey,
		GreyAlpha,
		Rgb,
		RgbAlpha
	}colour_type;

	png_in(std::istream&);
	~png_in();

	void expand_depth_to_8();
	void unpack_to_bytes();

	void get_raw_pixel_lines(unsigned char**, int);
	void get_raw_pixel_lines(unsigned short**, int);

	void read_end();

	int depth;
	int width, height;
	std::string error_string;

	std::istream& i;

	png_struct_def* png_ptr;
	png_info_struct* info_ptr, *end_info;


};

struct png_out
{
	typedef enum
	{
		Grey,
		GreyAlpha,
		Rgb,
		RgbAlpha
	}colour_type;

	png_out(int w, int h, colour_type t, int depth, std::ostream&);
	~png_out();

	void pack();
	void rgbx();

	void write_raw_pixel_lines(const std::vector<const unsigned char*>&);
	void write_raw_pixel_lines(const std::vector<const unsigned short*>&);

	void read_end();

	std::string error_string;

	std::ostream& o;

	png_struct_def* png_ptr;
	png_info_struct* info_ptr, *end_info;
};

////////////////////////////////////////////////////////////////////////////////
//
// Utility
//


template<class C, class D> void make_row_pointers(SubImage<C>& im, std::vector<D*>& rows)
{
	rows.resize(im.size().y);
	for(int r=0; r < im.size().y; r++)
		rows[r] = (D*)im[r];
}


template<class C, class D> void make_row_pointers(const SubImage<C>& im, std::vector<const D*>& rows)
{
	rows.resize(im.size().y);
	for(int r=0; r < im.size().y; r++)
		rows[r] = (const D*)im[r];
}

////////////////////////////////////////////////////////////////////////////////
//
// Read Greyscale PNG
//
template<class Dest, class Src_ish> struct readPNG_grey
{
	static void read(SubImage<Dest>& im, png_in& png)
	{
		if(png.depth < 8)
			png.expand_depth_to_8();

		std::vector<Src_ish> row_buf(png.width);
		Src_ish *row = &row_buf[0];

		for(int r=0; r < png.height; r++)
		{
			png.get_raw_pixel_lines(&row, 1);
			Pixel::ConvertPixels<Src_ish, Dest>::convert(row, im[r], png.width);
		}
	}
};

template<class Dest> struct readPNG_grey<Dest, bool>
{
	static void read(SubImage<Dest>& im, png_in& png)
	{
		readPNG_grey<Dest, byte>::read(im, png);
	}
};

template<> struct readPNG_grey<bool, bool>
{
	static void read(SubImage<bool>& im, png_in& png)
	{
		png.unpack_to_bytes();
		std::vector<byte*> rows;
		make_row_pointers(im, rows);
		png.get_raw_pixel_lines(&rows[0], png.height);
	}
};

template<> struct readPNG_grey<byte, unsigned char>
{
	static void read(SubImage<byte>& im, png_in& png)
	{
		if(png.depth < 8)
			png.expand_depth_to_8();

		std::vector<byte*> rows;
		make_row_pointers(im, rows);

		png.get_raw_pixel_lines(&rows[0], png.height);
	}
};


template<> struct readPNG_grey<unsigned short, unsigned short>
{
	static void read(SubImage<unsigned short>& im, png_in& png)
	{
		std::vector<unsigned short*> rows;
		make_row_pointers(im, rows);
		png.get_raw_pixel_lines(&rows[0], png.height);
	}
};

////////////////////////////////////////////////////////////////////////////////
//
// Read RGB PNG
//

template<class Dest, class Src> struct readPNG_rgb
{
	static void read(SubImage<Dest>& im, png_in& png)
	{
		std::vector<Src> row_buf(png.width*3);
		Src* row = &row_buf[0];

		for(int r=0; r < png.height; r++)
		{
			png.get_raw_pixel_lines(&row, 1);
			Pixel::ConvertPixels<Rgb<Src>, Dest >::convert(reinterpret_cast<Rgb<Src>*>(row), im[r], png.width);
		}
	}
};

template<> struct readPNG_rgb<Rgb<byte>, byte>
{
	static void read(SubImage<Rgb<byte> >& im, png_in& png)
	{
		std::vector<byte*> rows;
		make_row_pointers(im, rows);
		png.get_raw_pixel_lines(&rows[0]  , png.height);
	}
};

template<> struct readPNG_rgb<Rgb<unsigned short>, unsigned short>
{
	static void read(SubImage<Rgb<unsigned short> >& im, png_in& png)
	{
		std::vector<unsigned short*> rows;
		make_row_pointers(im, rows);
		png.get_raw_pixel_lines(&rows[0], png.height);
	}
};

////////////////////////////////////////////////////////////////////////////////
//
// Read RGBA PNG
//

template<class Dest, class Src> struct readPNG_rgba
{
	static void read(SubImage<Dest>& im, png_in& png)
	{
		std::vector<Src> row_buf(png.width*4);
		Src* row = &row_buf[0];

		for(int r=0; r < png.height; r++)
		{
			png.get_raw_pixel_lines(&row, 1);
			Pixel::ConvertPixels<Rgba<Src>, Dest >::convert(reinterpret_cast<Rgba<Src>*>(row), im[r], png.width);
		}
	}
};

template<> struct readPNG_rgb<Rgba<byte>, byte>
{
	static void read(SubImage<Rgba<byte> >& im, png_in& png)
	{
		std::vector<byte*> rows;
		make_row_pointers(im, rows);
		png.get_raw_pixel_lines(&rows[0]  , png.height);
	}
};

template<> struct readPNG_rgb<Rgba<unsigned short>, unsigned short>
{
	static void read(SubImage<Rgba<unsigned short> >& im, png_in& png)
	{
		std::vector<unsigned short*> rows;
		make_row_pointers(im, rows);
		png.get_raw_pixel_lines(&rows[0], png.height);
	}
};

////////////////////////////////////////////////////////////////////////////////
//
// Runtime switch on type
//

template<class C> void readPNG(SubImage<C>& im, png_in& png)
{
	switch(png.colour_type)
	{	
		case png_in::Grey:	
				if(png.depth == 16)
					readPNG_grey<C, unsigned short>::read(im, png);
				else if(png.depth == 1)
					readPNG_grey<C, bool>::read(im, png);
				else
					readPNG_grey<C, unsigned char>::read(im, png);
			break;

		case png_in::GreyAlpha:
				throw Exceptions::Image_IO::UnsupportedImageSubType("PNG", "Grey-alpha not yet supported");
			break;

		case png_in::Rgb:
				if(png.depth == 16)
					readPNG_rgb<C, unsigned short>::read(im, png);
				else
					readPNG_rgb<C, unsigned char>::read(im, png);
			break;

		case png_in::RgbAlpha:
				if(png.depth == 16)
					readPNG_rgba<C, unsigned short>::read(im, png);
				else
					readPNG_rgba<C, unsigned char>::read(im, png);
			break;
	}
	
	//This clears the stream of PNGs
	png.read_end();
}

////////////////////////////////////////////////////////////////////////////////
//
// Adapter for various image manifestations
//

template <class T> void readPNG(SubImage<T>& im, std::istream& in)
{
	png_in png(in);
	ImageRef size(png.width, png.height);
	if (im.size() != size)
		throw Exceptions::Image_IO::ImageSizeMismatch(size, im.size());
	readPNG(im, png);
}

template <class T> void readPNG(Image<T>& im, std::istream& in)
{
	png_in png(in);
	ImageRef size(png.width, png.height);
	im.resize(size);
	readPNG(im, png);
}








////////////////////////////////////////////////////////////////////////////////
//
// How to convert Misc types in to PNG compatible types
//

template<int Components, bool use_16bit> struct PNGType;
template<> struct PNGType<1,0>{ typedef byte type; static const png_out::colour_type Colour = png_out::Grey; static const int Depth=8;};
template<> struct PNGType<1,1>{ typedef unsigned short type; static const png_out::colour_type Colour = png_out::Grey; static const int Depth=16;};
template<> struct PNGType<3,0>{ typedef Rgb<byte> type; static const png_out::colour_type Colour = png_out::Rgb; static const int Depth=8;};
template<> struct PNGType<3,1>{ typedef Rgb<unsigned short> type; static const png_out::colour_type Colour = png_out::Rgb; static const int Depth=16;};
template<> struct PNGType<4,0>{ typedef Rgba<byte> type; static const png_out::colour_type Colour = png_out::RgbAlpha; static const int Depth=8;};
template<> struct PNGType<4,1>{ typedef Rgba<unsigned short> type; static const png_out::colour_type Colour = png_out::RgbAlpha; static const int Depth=16;};


template<class T> struct PNGWriter
{
	static void write(const SubImage<T>& i, std::ostream& o)
	{
		typedef PNGType<Pixel::Component<T>::count, Internal::save_default<T>::use_16bit> PNGInfo;

		typedef typename PNGInfo::type Out;
		typedef typename Pixel::Component<Out>::type Comp;
		
		png_out po(i.size().x, i.size().y, (png_out::colour_type)PNGInfo::Colour, PNGInfo::Depth, o);
		std::vector<Out> row_buf(i.size().x);
		std::vector<const Comp*> rows;
		rows.push_back(reinterpret_cast<const Comp*>(&row_buf[0]));


		
		for(int r=0; r < i.size().y ; r++)
		{
			Pixel::ConvertPixels<T,Out>::convert(i[r], &row_buf[0], i.size().x);
			po.write_raw_pixel_lines(rows);
		}
	}
};

template<class T> void writePNG(const SubImage<T>& i, std::ostream& o)
{
	PNGWriter<T>::write(i, o);
}

////////////////////////////////////////////////////////////////////////////////
//
// Types which require no conversions
//

template<> struct PNGWriter<bool>
{
	static void write(const SubImage<bool>& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::Grey, 1, o);
		po.pack();
		std::vector<const byte*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};

template<> struct PNGWriter<byte>
{
	static void write(const SubImage<byte>& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::Grey, 8, o);
		std::vector<const byte*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};

template<> struct PNGWriter<unsigned short>
{
	static void write(const SubImage<unsigned short>& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::Grey, 16, o);
		std::vector<const unsigned short*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};


template<> struct PNGWriter<Rgb8>
{
	static void write(const SubImage<Rgb<byte> >& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::Rgb, 8, o);
		po.rgbx();
		std::vector<const byte*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};


template<> struct PNGWriter<Rgb<byte> >
{
	static void write(const SubImage<Rgb<byte> >& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::Rgb, 8, o);
		std::vector<const byte*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};

template<> struct PNGWriter<Rgb<unsigned short> >
{
	static void write(const SubImage<Rgb<unsigned short> >& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::Rgb, 16, o);
		std::vector<const byte*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};

template<> struct PNGWriter<Rgba<byte> >
{
	static void write(const SubImage<Rgba<byte> >& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::RgbAlpha, 8, o);
		std::vector<const byte*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};

template<> struct PNGWriter<Rgba<unsigned short> >
{
	static void write(const SubImage<Rgba<unsigned short> >& i, std::ostream& o)
	{
		png_out po(i.size().x, i.size().y, png_out::RgbAlpha, 16, o);
		std::vector<const byte*> rows;
		make_row_pointers(i, rows);
		po.write_raw_pixel_lines(rows);
	}
};


















}}
#endif
