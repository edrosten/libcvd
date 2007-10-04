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
#ifndef PNM_JPEG_H
#define PNM_JPEG_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
namespace JPEG
{
extern "C"{
#include <jpeglib.h>
};

	class jpeg_in
	{
		public:
			jpeg_in(std::istream&);
			int channels(){return m_channels;}
			long  x_size() const {return xs;}
			long  y_size() const {return ys;}
			long  elements_per_line() const {return xs * m_channels;}
			void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
			~jpeg_in();
			
		private:
			long	xs, ys;
			int	m_channels;
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			std::istream&	i;
	};

  template <class T, class S> struct JPEGReader {
    static void read(BasicImage<T>& im, jpeg_in& jpeg) {
      std::vector<S> rowbuf(jpeg.x_size());
      for (int r=0; r<jpeg.y_size(); r++) {
	jpeg.get_raw_pixel_lines((byte*)&rowbuf[0], 1);
	Pixel::ConvertPixels<S,T>::convert(&rowbuf[0], im[r], jpeg.x_size());
      }
    }
  };

  template <> struct JPEGReader<byte,byte> {
    static void read(BasicImage<byte>& im, jpeg_in& jpeg) {
      jpeg.get_raw_pixel_lines(im.data(), jpeg.y_size());
    }
  };

  template <> struct JPEGReader<Rgb<byte>, Rgb<byte> > {
    static void read(BasicImage<Rgb<byte> >& im, jpeg_in& jpeg) {
      jpeg.get_raw_pixel_lines((byte*)im.data(), jpeg.y_size());
    }
  };

  template <class T> void readJPEG(Image<T>& im, std::istream& in)
  {
    jpeg_in jpeg(in);
    im.resize(ImageRef(jpeg.x_size(), jpeg.y_size()));
    if (jpeg.channels() == 3)
      JPEGReader<T,Rgb<byte> >::read(im, jpeg);
    else
      JPEGReader<T,byte>::read(im, jpeg);    
  }

  template <class T> void readJPEG(BasicImage<T>& im, std::istream& in)
  {
    jpeg_in jpeg(in);
	ImageRef size(jpeg.x_size(), jpeg.y_size());
	if(size != im.size())
	    throw Exceptions::Image_IO::ImageSizeMismatch(size, im.size());
	
    if (jpeg.channels() == 3)
      JPEGReader<T,Rgb<byte> >::read(im, jpeg);
    else
      JPEGReader<T,byte>::read(im, jpeg);    
  }


	class jpeg_out
	{
		public:
	  jpeg_out(std::ostream&, int  xsize, int ysize, int ch, const std::string& comm="");
			int channels(){return m_channels;}
			long  x_size() const {return xs;}
			long  y_size() const {return ys;}
			long  elements_per_line() const {return xs * m_channels;}
			void 	write_raw_pixel_lines(const unsigned char*, unsigned long);
			~jpeg_out();
			
		private:
			long	xs, ys;
			int	m_channels;
			struct jpeg_compress_struct cinfo;
			struct jpeg_error_mgr jerr;
			std::ostream& 	o;
	};

  template <class T, class S> struct JPEGWriter {
    static void write(const BasicImage<T>& im, std::ostream& out) {
      jpeg_out jpeg(out, im.size().x, im.size().y, Pixel::Component<S>::count);      
      std::auto_ptr<S> rowbuf(new S[im.size().x]);
      for (int r=0; r<jpeg.y_size(); r++) {
	Pixel::ConvertPixels<T,S>::convert(im[r], rowbuf.get(), im.size().x);
	jpeg.write_raw_pixel_lines((const byte*)rowbuf.get(), 1);
      }
    }
  };
  
  template <> struct JPEGWriter<byte,byte> {
    static void write(const BasicImage<byte>& im, std::ostream& out) {
      jpeg_out jpeg(out, im.size().x, im.size().y, 1);      
      jpeg.write_raw_pixel_lines(im.data(), im.size().y);
    }
  };

  template <> struct JPEGWriter<Rgb<byte>,Rgb<byte> > {
    static void write(const BasicImage<Rgb<byte> >& im, std::ostream& out) {
      jpeg_out jpeg(out, im.size().x, im.size().y, 3);      
      jpeg.write_raw_pixel_lines((const byte*)im.data(), im.size().y);
    }
  };

  template <class T, int C=Pixel::Component<T>::count> struct JPEGWriterChooser {
    typedef JPEGWriter<T,byte> type;
  };

  template <class T> struct JPEGWriterChooser<T,3> {
    typedef JPEGWriter<T,Rgb<byte> > type;
  };

  template <class T> void writeJPEG(const BasicImage<T>& im, std::ostream& out)
  {
    typedef typename JPEGWriterChooser<T>::type Writer;
    Writer::write(im, out);
  }


}
}

#endif
