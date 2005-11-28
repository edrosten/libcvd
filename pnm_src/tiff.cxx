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
#include "pnm_src/tiff.h"
#include "cvd/image_io.h"
#include <tiffio.h>
#include <memory>

using namespace CVD;
using namespace TIFF;
using namespace CVD::Exceptions::Image_IO;
using namespace std;

tsize_t tiff_in::read(thandle_t vis, tdata_t data, tsize_t count)
{
	tiff_in* i = (tiff_in*)vis;
	i->i.read((char*)data, count);
	return i->i.gcount();
}

tsize_t tiff_in::write(thandle_t vis, tdata_t data, tsize_t count)
{
	return 0;
}

toff_t tiff_in::seek(thandle_t vis, toff_t off, int dir)
{
	tiff_in* i = (tiff_in*)vis;

	if(dir == SEEK_SET)
		i->i.seekg(i->start +  off, ios_base::beg);
	else if(dir == SEEK_CUR)
		i->i.seekg(off, ios_base::cur);
	else if(dir == SEEK_END)
		i->i.seekg(off, ios_base::end);

	return i->i.tellg() - i->start;
}

toff_t tiff_in::size(thandle_t vis)
{
	tiff_in* ii = (tiff_in*)vis;
	return ii->length;
}

int tiff_in::close(thandle_t vis)
{
	return 0;
}

int tiff_in::map(thandle_t, tdata_t*, toff_t*)
{
	return 0;
}

void tiff_in::unmap(thandle_t, tdata_t, toff_t)
{
}


static const int error_size=512;
static char error_msg[error_size]="";

static void tiff_error_handler(const char* module, const char* fmt, va_list ap)
{
	int n = vsnprintf(error_msg, error_size, fmt, ap);
	if(n == error_size)
		error_msg[n-1] = 0;
}

void tiff_in::get_raw_pixel_lines(unsigned char* data, unsigned long nlines)
{
	if(m_is_2_byte)
		throw ReadTypeMismatch(1);
	
	if(!use_cooked_rgba_interface)
	{
		for(unsigned long i=0; i < nlines; i++, row++, data+=xs*m_channels)
		{
				int r = TIFFReadScanline(tif, (void*)data, row);
				if(r == -1)
					throw MalformedImage(error_msg);

				if(inverted_grey)
				{
					for(long j=0; j < xs; j+=m_channels)
						data[j] = 255 - data[j];
				}
		}
	}
	else
	{
		if(row+nlines > (unsigned long)ys)
			throw InternalLibraryError("CVD", "Read past end of image.");

		uint32* raster = raster_data + row*xs;
		uint32* end = raster + nlines * xs;
	
		for(;raster < end; raster++)
		{
			*data++ = TIFFGetR(*raster);
			*data++ = TIFFGetG(*raster);
			*data++ = TIFFGetB(*raster);
			*data++ = TIFFGetA(*raster);
		}

		row += nlines;	
	}
}

void tiff_in::get_raw_pixel_lines(unsigned short* data, unsigned long nlines)
{
	if(!m_is_2_byte)
		throw ReadTypeMismatch(0);

	for(unsigned long i=0; i < nlines; i++, row++, data+=xs*m_channels)
	{
		int r = TIFFReadScanline(tif, (void*)data, row);
		if(r == -1)
			throw MalformedImage(error_msg);

		if(inverted_grey)
		{
			for(long j=0; j < xs; j++)
				data[j] = 255 - data[j];
		}
	}
}

tiff_in::~tiff_in()
{	
	TIFFClose(tif);
	delete[] raster_data;
}


tiff_in::tiff_in(istream& is)
:i(is),row(0),raster_data(0)
{
	TIFFSetErrorHandler(tiff_error_handler);
	start = i.tellg();

	if(start == -1)
		throw UnseekableIstream("TIFF");
	
	i.seekg(0, ios_base::end);

	length = i.tellg() - start;
	i.seekg(start, ios_base::beg);


	tif = TIFFClientOpen("std::istream", "r", this, 
						 read, write, seek, close, size, map, unmap);


	if(tif == NULL)
		throw MalformedImage(error_msg);

	unsigned int  w=0, h=0, bps=0, spp=0, photo=0, pl_type=0;

		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
     	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);	
     	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
     	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
     	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);	
     	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &pl_type);

	xs = w;
	ys = h;
	m_channels = 4;
	use_cooked_rgba_interface=1;
	m_is_2_byte = 0;

	//We can use out own reading interface if:

	if((photo == PHOTOMETRIC_RGB  || photo == PHOTOMETRIC_MINISWHITE || photo == PHOTOMETRIC_MINISBLACK) && 			
		(spp <= 4) &&
		(pl_type == PLANARCONFIG_CONTIG) &&				//Data is contiguous (not nasty planar stuff)
		(bps == 8 || bps == 16))						//8 or 16 bits per sample
	{
		use_cooked_rgba_interface = 0;
		m_is_2_byte = (bps == 16);
		m_channels = spp;
		inverted_grey = (photo == PHOTOMETRIC_MINISWHITE);
	}
	
	if(use_cooked_rgba_interface)
	{
		raster_data = new uint32[xs*ys];
		//Read the whole image
		if(TIFFReadRGBAImageOriented(tif, xs, ys, raster_data, 0, ORIENTATION_TOPLEFT) == -1)
			throw MalformedImage(error_msg);
	}
}


