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
#ifndef PNM_SAVE_POSTSCRIPT_H
#define PNM_SAVE_POSTSCRIPT_H

#include <iostream>
#include <string>
#include <memory>
#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/internal/convert_pixel_types.h>

namespace CVD
{
namespace PS
{

	class ps_out
	{
		public:
			ps_out(std::ostream&, int  xsize, int ysize, int ch);

			void 	write_raw_pixel_lines(const unsigned char*, unsigned long);
			virtual ~ps_out();
			int channels(){return m_channels;}
			long  x_size() const {return xs;}
			long  y_size() const {return ys;}

		protected:
			long	xs, ys;
			int	m_channels;
			std::string bytes_to_base85(int n);
			void output_header();
			int lines;
			unsigned char buf[4];
			int  num_in_buf;
			ps_out(std::ostream&);
			std::ostream& 	o;
	};

	class eps_out: public ps_out
	{
		public:
			virtual ~eps_out();
			eps_out(std::ostream&, int  xsize, int ysize, int ch);
	};

	template <class T, class S> struct PSWriter {
	  static void write(const BasicImage<T>& im, ps_out& ps) {
	    std::auto_ptr<S> rowbuf(new S[im.size().x]);
	    for (int r=0; r<ps.y_size(); r++) {
	      Pixel::ConvertPixels<T,S>::convert(im[r], rowbuf.get(), im.size().x);
	      ps.write_raw_pixel_lines((const byte*)rowbuf.get(), 1);
	    }	    
	  }
	};

	template <> struct PSWriter<byte,byte> {
	  static void write(const BasicImage<byte>& im, ps_out& ps) {
	    ps.write_raw_pixel_lines(im.data(), ps.y_size());
	  }
	};

	template <> struct PSWriter<Rgb<byte>,Rgb<byte> > {
	  static void write(const BasicImage<Rgb<byte> >& im, ps_out& ps) {
	    ps.write_raw_pixel_lines((const byte*)im.data(), ps.y_size());
	  }
	};
	
	template <class T, int C=Pixel::Component<T>::count> struct PSWriterChooser{
	  typedef PSWriter<T,byte> type;
	  enum {channels = 1};
	};
	template <class T> struct PSWriterChooser<T,3> {
	  typedef PSWriter<T,Rgb<byte> > type;
	  enum {channels = 3};
	};

	template <class T> void writePS(const BasicImage<T>& im, std::ostream& out) {
	  typedef typename PSWriterChooser<T>::type Writer;
	  ps_out ps(out, im.size().x, im.size().y, PSWriterChooser<T>::channels);
	  Writer::write(im, ps);	  
	}
	template <class T> void writeEPS(const BasicImage<T>& im, std::ostream& out) {
	  typedef typename PSWriterChooser<T>::type Writer;
	  eps_out eps(out, im.size().x, im.size().y, PSWriterChooser<T>::channels);
	  Writer::write(im, eps);  
	}

}
}
#endif
