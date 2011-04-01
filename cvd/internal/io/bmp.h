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
		using Internal::TypeList;
		using Internal::Head;
		
		class ReadPimpl;
		class Reader
		{
			public:
				Reader(std::istream& i);
				~Reader();

				ImageRef size();
				bool top_row_first();
				
				void get_raw_pixel_line(unsigned char*);
				void get_raw_pixel_line(Rgb<unsigned char>*);

				std::string datatype();
				std::string name();

				typedef TypeList<byte, 
				        TypeList<Rgb<byte>, 
						                   Head> > Types;

			private:
				std::auto_ptr<ReadPimpl> t;
		};
/*
		class WritePimpl;
		class Writer
		{
			public:
				Writer(std::ostream&, ImageRef size, const std::string& type, const std::map<std::string, Parameter<> >& p);
				~Writer();

				void write_raw_pixel_line(const byte*);
				void write_raw_pixel_line(const Rgb<byte>*);

				template<class Incoming> struct Outgoing
				{		
					typedef byte type;
				};		

			protected:
				std::auto_ptr<WritePimpl> t; 
		};

		template<class C> struct Writer::Outgoing<Rgb<C> > 
		{
			typedef Rgb<byte> type;
		};


		template<class C> struct Writer::Outgoing<Rgba<C> > 
		{
			typedef Rgb<byte> type;
		};

		template<> struct Writer::Outgoing<Rgb8> 
		{
			typedef Rgb<byte> type;
		};
*/

		void writeBMPHeader(unsigned int width, unsigned int height, unsigned int channels, std::ostream& out);

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
