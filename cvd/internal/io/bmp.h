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

				static const int top_row_first=0;

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

	}
}

#endif
