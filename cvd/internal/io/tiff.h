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
#ifndef PNM_TIFF
#define PNM_TIFF

#include <iostream>
#include <string>
#include <stdarg.h>
#include <cvd/image.h>
#include <cvd/internal/load_and_save.h>
#include <cvd/internal/convert_pixel_types.h>
namespace CVD
{
namespace TIFF
{

#include <tiffio.h>

	class tiff_in 
	{
		public:
			tiff_in(std::istream&);
			void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
			void get_raw_pixel_lines(unsigned short*, unsigned long nlines);
			~tiff_in();
			int channels(){return m_channels;}
			long  x_size() const {return xs;}
			long  y_size() const {return ys;}
			long  elements_per_line() const {return xs * m_channels;}
			bool is_2_byte()const {return m_is_2_byte;}
			
		private:
			long	xs, ys;
			int	m_channels;
			bool m_is_2_byte;

			TIFF* tif;
			std::streamoff start, length;
			std::istream&	i;
			unsigned int row;
			
			bool use_cooked_rgba_interface;
			bool inverted_grey;
			uint32 *raster_data;
			
			static tsize_t write(thandle_t vis, tdata_t data, tsize_t count);
			static tsize_t read(thandle_t vis, tdata_t data, tsize_t count);
			static toff_t seek(thandle_t vis, toff_t off, int dir);
			static toff_t size(thandle_t vis);
			static int close(thandle_t vis);
			static int map(thandle_t, tdata_t*, toff_t*);
			static void unmap(thandle_t, tdata_t, toff_t);


	};
	
	template <class T, class S, int N> struct TIFFReader;

	template <class T, class S> struct TIFFReader<T,S,1> {
	  static void read(BasicImage<T>& im, tiff_in& tiff) {
	    std::auto_ptr<S> rowbuf(new S[tiff.x_size()]);
	    for (int r=0; r<tiff.y_size(); r++) {
	      tiff.get_raw_pixel_lines(rowbuf.get(), 1);
	      Pixel::ConvertPixels<S,T>::convert(rowbuf.get(), im[r], tiff.x_size());
	    }	    
	  }
	};
	template <class T, class S> struct TIFFReader<T,S,3> {
	  static void read(BasicImage<T>& im, tiff_in& tiff) {
	    std::auto_ptr<Rgb<S> > rowbuf(new Rgb<S>[tiff.x_size()]);
	    for (int r=0; r<tiff.y_size(); r++) {
	      tiff.get_raw_pixel_lines((S*)rowbuf.get(), 1);
	      Pixel::ConvertPixels<Rgb<S>,T>::convert(rowbuf.get(), im[r], tiff.x_size());
	    }	    
	  }
	};

	template <class T, class S> struct TIFFReader<T,S,4> {
	  static void read(BasicImage<T>& im, tiff_in& tiff) {
	    std::auto_ptr<Rgba<S> > rowbuf(new Rgba<S>[tiff.x_size()]);
	    for (int r=0; r<tiff.y_size(); r++) {
	      tiff.get_raw_pixel_lines((S*)rowbuf.get(), 1);
	      Pixel::ConvertPixels<Rgba<S>,T>::convert(rowbuf.get(), im[r], tiff.x_size());
	    }	    
	  }
	};

	template <class S> struct TIFFReader<S,S,1> {
	  static void read(BasicImage<S>& im, tiff_in& tiff) {
	    tiff.get_raw_pixel_lines(im.data(), tiff.y_size());
	  }
	};

	template <class S> struct TIFFReader<Rgb<S>,S,3> {
	  static void read(BasicImage<Rgb<S> >& im, tiff_in& tiff) {
	    tiff.get_raw_pixel_lines((S*)im.data(), tiff.y_size());
	  }
	};

	template <class S> struct TIFFReader<Rgba<S>,S,4> {
	  static void read(BasicImage<Rgba<S> >& im, tiff_in& tiff) {
	    tiff.get_raw_pixel_lines((S*)im.data(), tiff.y_size());
	  }
	};

	template <class T> void readTIFF(BasicImage<T>& im, std::istream& in)
	{
	  tiff_in tiff(in);
	  ImageRef size(tiff.x_size(), tiff.y_size());
	  if (im.size() != size)
	    throw Exceptions::Image_IO::ImageSizeMismatch(size, im.size());
	  readTIFF(im, tiff);
	}

	template <class T> void readTIFF(Image<T>& im, std::istream& in)
	{
	  tiff_in tiff(in);
	  ImageRef size(tiff.x_size(), tiff.y_size());
	  im.resize(size);
	  readTIFF(im, tiff);
	}

	template <class T> void readTIFF(BasicImage<T>& im, tiff_in& tiff)
	{
	  if (tiff.is_2_byte()) {
	    if (tiff.channels() == 4)
	      TIFFReader<T,unsigned short,4>::read(im,tiff);
	    else if (tiff.channels() == 3)
	      TIFFReader<T,unsigned short,3>::read(im,tiff);
	    else if (tiff.channels() == 1)
	      TIFFReader<T,unsigned short,1>::read(im,tiff);
	    else if (tiff.channels() == 2)
	      throw Exceptions::Image_IO::UnsupportedImageSubType("TIFF", "Grey-alpha not yet supported");
	  } else {
	    if (tiff.channels() == 4)
	      TIFFReader<T,byte,4>::read(im,tiff);
	    else if (tiff.channels() == 3)
	      TIFFReader<T,byte,3>::read(im,tiff);
	    else if (tiff.channels() == 1)
	      TIFFReader<T,byte,1>::read(im,tiff);
	    else if (tiff.channels() == 2)
	      throw Exceptions::Image_IO::UnsupportedImageSubType("TIFF", "Grey-alpha not yet supported");
	  }
	}
}
}
#endif
