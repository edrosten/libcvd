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
#ifndef PNM_GROK_H
#define PNM_GROK_H

#include <iostream>
#include <string>
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/image.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
  namespace PNM
  {
    class pnm_in
    {
    public:
      pnm_in(std::istream&);
      bool is_2_byte()const {return m_is_2_byte;}
      int channels(){return m_channels;}
      long  x_size() const {return xs;}
      long  y_size() const {return ys;}
      long  elements_per_line() const {return xs * m_channels;}
      void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
      void get_raw_pixel_lines(unsigned short*, unsigned long nlines);

			
    private:
      std::istream&	i;
      bool 	is_text;
      int     type, maxval;
      int     lines_so_far;
      void	read_header();
      bool    can_proc_lines(unsigned long);
      long	xs, ys;
      bool 	m_is_2_byte;
      int	m_channels;
    };

    template <class T, class S, int N> struct PNMReader;
    template <class T, class S> struct PNMReader<T,S,3> {
      typedef Rgb<S> array;
      static void readPixels(BasicImage<T>& im, pnm_in& pnm) {
	std::auto_ptr<array> rowbuf(new array[pnm.x_size()]);
	for (int r=0; r<pnm.y_size(); r++) {
	  pnm.get_raw_pixel_lines((S*)rowbuf.get(), 1);
	  Pixel::ConvertPixels<array, T>::convert(rowbuf.get(), im[r], pnm.x_size());
	}
      }
    };
  
    template <class T, class S> struct PNMReader<T,S,1> {
      static void readPixels(BasicImage<T>& im, pnm_in& pnm) {
	std::auto_ptr<S> rowbuf(new S[pnm.x_size()]);
	for (int r=0; r<pnm.y_size(); r++) {
	  pnm.get_raw_pixel_lines(rowbuf.get(), 1);
	  Pixel::ConvertPixels<S, T>::convert(rowbuf.get(), im[r], pnm.x_size());
	}
      }
    };

    template <> struct PNMReader<Rgb<byte>,byte,3> {
      static void readPixels(BasicImage<Rgb<byte> >& im, pnm_in& pnm) {
	pnm.get_raw_pixel_lines((byte*)im.data(), pnm.y_size());
      }
    };

    template <> struct PNMReader<byte,byte,1> {
      static void readPixels(BasicImage<byte>& im, pnm_in& pnm) {
	pnm.get_raw_pixel_lines(im.data(), pnm.y_size());
      }
    };

    template <> struct PNMReader<Rgb<unsigned short>,unsigned short,3> {
      static void readPixels(BasicImage<Rgb<unsigned short> >& im, pnm_in& pnm) {
	pnm.get_raw_pixel_lines((unsigned short*)im.data(), pnm.y_size());
      }
    };

    template <> struct PNMReader<unsigned short,unsigned short,1> {
      static void readPixels(BasicImage<unsigned short>& im, pnm_in& pnm) {
	pnm.get_raw_pixel_lines(im.data(), pnm.y_size());
      }
    };
  
    template <class T> void readPNM(BasicImage<T>& im, pnm_in& pnm)
    {
      if (pnm.is_2_byte()) {
	if (pnm.channels() == 3)
	  PNMReader<T,unsigned short,3>::readPixels(im, pnm);
	else 
	  PNMReader<T,unsigned short,1>::readPixels(im, pnm);
      } else {
	if (pnm.channels() == 3)
	  PNMReader<T,unsigned char,3>::readPixels(im, pnm);
	else 
	  PNMReader<T,unsigned char,1>::readPixels(im, pnm);
      }
    }
	
	template <class T> void readPNM(BasicImage<T>&im, std::istream& in)
	{
      pnm_in pnm(in);
	  ImageRef size(pnm.x_size(), pnm.y_size());

	  if(size != im.size())
	    throw Exceptions::Image_IO::ImageSizeMismatch(size, im.size());
	
		readPNM(im, pnm);

	}

	template <class T> void readPNM(Image<T>&im, std::istream& in)
	{
      pnm_in pnm(in);
      im.resize(ImageRef(pnm.x_size(), pnm.y_size()));
	  readPNM(im, pnm);
	}


    void writePNMHeader(std::ostream& out, int channels, ImageRef size, int maxval, bool text, const std::string& comments);

    void writePNMPixels(std::ostream& out, const byte* data, size_t count, bool text);
    void writePNMPixels(std::ostream& out, const unsigned short* data, size_t count, bool text);
    
    template <class T, int N, bool SixteenBit=CVD::Internal::save_default<typename Pixel::Component<T>::type>::use_16bit> struct PNMWriter;
    template <class T> struct PNMWriter<T,3,true> {
      typedef Rgb<unsigned short> array;
      static void write(const BasicImage<T>& im, std::ostream& out) {
	writePNMHeader(out, 3, im.size(), 65535, false, "");	
	std::auto_ptr<array> rowbuf(new array[im.size().x]);
	for (int r=0; r<im.size().y; r++) {
	  Pixel::ConvertPixels<T, array>::convert(im[r], rowbuf.get(), im.size().x);
	  writePNMPixels(out, (const unsigned short*)rowbuf.get(), im.size().x*3, false);
	}	
      }
    };

    template <class T> struct PNMWriter<T,3,false> {
      typedef Rgb<byte> array;
      static void write(const BasicImage<T>& im, std::ostream& out) {
	writePNMHeader(out, 3, im.size(), 255, false, "");	
	std::auto_ptr<array> rowbuf(new array[im.size().x]);
	for (int r=0; r<im.size().y; r++) {
	  Pixel::ConvertPixels<T, array>::convert(im[r], rowbuf.get(), im.size().x);
	  writePNMPixels(out, (const byte*)rowbuf.get(), im.size().x*3, false);
	}	
      }
    };

    template <class T> struct PNMWriter<T,1,true> {
      typedef unsigned short S;
      static void write(const BasicImage<T>& im, std::ostream& out) {
	writePNMHeader(out, 1, im.size(), 255, false, "");	
	std::auto_ptr<S> rowbuf(new S[im.size().x]);
	for (int r=0; r<im.size().y; r++) {
	  Pixel::ConvertPixels<T, S>::convert(im[r], rowbuf.get(), im.size().x);
	  writePNMPixels(out, rowbuf.get(), im.size().x, false);
	}	
      }
    };

    template <class T> struct PNMWriter<T,1,false> {
      typedef byte S;
      static void write(const BasicImage<T>& im, std::ostream& out) {
	writePNMHeader(out, 1, im.size(), 255, false, "");	
	std::auto_ptr<S> rowbuf(new S[im.size().x]);
	for (int r=0; r<im.size().y; r++) {
	  Pixel::ConvertPixels<T, S>::convert(im[r], rowbuf.get(), im.size().x);
	  writePNMPixels(out, rowbuf.get(), im.size().x, false);
	}	
      }
    };

    template <> struct PNMWriter<byte,1> {
      static void write(const BasicImage<byte>& im, std::ostream& out) {
	writePNMHeader(out, 1, im.size(), 255, false, "");	
	writePNMPixels(out, im.data(), im.totalsize(), false);
      }
    };

    template <> struct PNMWriter<Rgb<byte>,3> {
      static void write(const BasicImage<Rgb<byte> >& im, std::ostream& out) {
	writePNMHeader(out, 3, im.size(), 255, false, "");	
	writePNMPixels(out, (const byte*)im.data(), im.totalsize()*3, false);
      }
    };

    template <> struct PNMWriter<unsigned short,1> {
      static void write(const BasicImage<unsigned short>& im, std::ostream& out) {
	writePNMHeader(out, 1, im.size(), 65535, false, "");	
	writePNMPixels(out, im.data(), im.totalsize(), false);
      }
    };

    template <> struct PNMWriter<Rgb<unsigned short>,3> {
      static void write(const BasicImage<Rgb<unsigned short> >& im, std::ostream& out) {
	writePNMHeader(out, 3, im.size(), 65535, false, "");	
	writePNMPixels(out, (const unsigned short*)im.data(), im.totalsize()*3, false);
      }
    };

    template <class T> void writePNM(const BasicImage<T>& im, std::ostream& out) {
      PNMWriter<T, Pixel::Component<T>::count == 3 ? 3 : 1>::write(im, out);
    }
  }
}
#endif
