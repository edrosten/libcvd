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
#ifndef BMP_H
#define BMP_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/internal/load_and_save.h>
#include <cvd/internal/simple_vector.h>

namespace CVD {
  namespace BMP {
    void writeBMPHeader(unsigned int width, unsigned int height, unsigned int channels, std::ostream& out);
    void readBMPHeader(unsigned int& width, unsigned int& height, unsigned int& channels, unsigned int& compression, std::istream& in);
      
    template <class T, int Channels> struct BMPReader;

    template <class T> struct BMPReader<T,1> {
      static void read(Image<T>& im, std::istream& in) {
	Internal::simple_vector<Rgb<byte> > palette(256);
	bool notgray = false;
	for (int i=0; i<256; i++) {
	  byte buf[4];
	  in.read((char*)buf,4);
	  palette[i].red = buf[2];
	  palette[i].green = buf[1];
	  palette[i].blue = buf[0];
	  if (buf[0] != i || buf[0] != buf[1] || buf[1] != buf[2])
	    notgray = true;
	}
	size_t rowSize = im.size().x;
	if (rowSize % 4)
	  rowSize += 4 - (rowSize%4);
	Internal::simple_vector<byte> rowbuf(rowSize);
	
	if (notgray) {
	  std::cerr << "not gray" << std::endl;
	  Internal::simple_vector<T> cvt(256);
	  Pixel::ConvertPixels<Rgb<byte>,T>::convert(&palette[0], &cvt[0], 256);
	  for (int r=im.size().y-1; r>=0; r--) {
          in.read((char*)&rowbuf[0], static_cast<std::streamsize>(rowSize));
	    for (int c=0; c<im.size().x; c++)
	      im[r][c] = cvt[rowbuf[c]];
	  } 
	} else {	  
	  for (int r=im.size().y-1; r>=0; r--) {
	    in.read((char*)&rowbuf[0], static_cast<std::streamsize>(rowSize));
	    Pixel::ConvertPixels<byte,T>::convert(&rowbuf[0], im[r], im.size().x);
	  }
	}
      }
    };
    template <class T> struct BMPReader<T,3> {
      static void read(Image<T>& im, std::istream& in) {
	size_t rowSize = im.size().x*3;
	if (rowSize % 4)
	  rowSize += 4 - (rowSize%4);
	Internal::simple_vector<byte> rowbuf(rowSize);
	for (int r=im.size().y-1; r>=0; r--) {
	  in.read((char*)&rowbuf[0], static_cast<std::streamsize>(rowSize));
	  for (int c=0; c<im.size().x*3; c+=3) {
	    byte tmp = rowbuf[c];
	    rowbuf[c] = rowbuf[c+2];
	    rowbuf[c+2] = tmp;
	  }
	  Pixel::ConvertPixels<Rgb<byte>, T>::convert((Rgb<byte>*)&rowbuf[0], im[r], im.size().x);
	}
      }
    };

    template <class T> void readBMP(Image<T>& im, std::istream& in) {
      unsigned int w,h,ch, comp;
      readBMPHeader(w,h,ch,comp,in);

      im.resize(ImageRef(w,h));
      if (ch == 1)
	BMPReader<T,1>::read(im, in);
      else
	BMPReader<T,3>::read(im,in);
    }

    template <class T> void readBMP(BasicImage<T>& im, std::istream& in) {
      unsigned int w,h,ch, comp;
      readBMPHeader(w,h,ch,comp,in);
      if (comp || (ch != 3 && ch != 1)) 
	throw CVD::Exceptions::Image_IO::UnsupportedImageType();
      
      ImageRef size(w, h);
      if(im.size() != size)
        throw Exceptions::Image_IO::ImageSizeMismatch(size, im.size());
      
      if (ch == 1)
	BMPReader<T,1>::read(im, in);
      else
	BMPReader<T,3>::read(im,in);
    }

    template <class T, int Channels> struct BMPWriter;
    template <class T> struct BMPWriter<T,1> {
      static void write(const BasicImage<T>& im, std::ostream& out) {
	writeBMPHeader(im.size().x, im.size().y, 1, out);
	int rowSize = im.size().x;
	if (rowSize % 4)
	  rowSize += 4 - (rowSize % 4);
	Internal::simple_vector<byte> rowbuf(rowSize);
	for (int r=im.size().y-1; r>=0; r--) {
	  Pixel::ConvertPixels<T,byte>::convert(im[r], &rowbuf[0], im.size().x);
	  out.write((const char*)&rowbuf[0], rowSize);
	}
      }
    };
    template <> struct BMPWriter<byte,1> {
      static void write(const BasicImage<byte>& im, std::ostream& out) {
	writeBMPHeader(im.size().x, im.size().y, 1, out);
	int pad = (im.size().x % 4) ? (4 - (im.size().x % 4)) : 0;
	char zeros[4]={0,0,0,0};
	for (int r=im.size().y-1; r>=0; r--) {
	  out.write((const char*)im[r], im.size().x);
	  if (pad)
	    out.write(zeros,pad);
	}
      }
    };

    template <class T> struct BMPWriter<T,3> {
      static void write(const BasicImage<T>& im, std::ostream& out) {
	writeBMPHeader(im.size().x, im.size().y, 3, out);
	int rowSize = im.size().x*3;
	if (rowSize % 4)
	  rowSize += 4 - (rowSize % 4);
	Internal::simple_vector<byte> rowbuf(rowSize);
	for (int r=im.size().y-1; r>=0; r--) {
	  Pixel::ConvertPixels<T,Rgb<byte> >::convert(im[r], (Rgb<byte>*)&rowbuf[0], im.size().x);
	  for (int c=0; c<im.size().x*3; c+=3) {
	    byte tmp = rowbuf[c];
	    rowbuf[c] = rowbuf[c+2];
	    rowbuf[c+2] = tmp;
	  }
	  out.write((const char*)&rowbuf[0], rowSize);
	}
      }
    };
    template <class T> struct BMPWriterChooser {  enum { channels = 1}; };
    template <class T> struct BMPWriterChooser<Rgb<T> > { enum { channels = 3}; };

    template <class T> void writeBMP(const BasicImage<T>& im, std::ostream& out) {
      BMPWriter<T,BMPWriterChooser<T>::channels>::write(im,out);
    }
  }
}

#endif
