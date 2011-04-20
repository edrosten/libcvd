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
#include <vector> 
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/image.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
  namespace PNM
  {
	class pnm_in;

	using CVD::Internal::TypeList;
	using CVD::Internal::Head;
	class Reader
	{
		public:
			Reader(std::istream&);
			~Reader();

			ImageRef size();
			bool top_row_first();

			void get_raw_pixel_line(bool*);
			void get_raw_pixel_line(unsigned char*);
			void get_raw_pixel_line(unsigned short*);
			void get_raw_pixel_line(Rgb<unsigned char>*);
			void get_raw_pixel_line(Rgb<unsigned short>*);

			std::string datatype();
			std::string name();

			typedef TypeList<bool,
					TypeList<byte,
					TypeList<unsigned short,
					TypeList<Rgb<byte>,
					TypeList<Rgb<unsigned short>,
										    	  Head> > > > > Types;

		private:
			std::auto_ptr<pnm_in> p;
		
	};





	////////////////////////////////////////////////////////////////////////////////
	//
	// PNM writing.
	//

	template<int isRgb, int isbyte> struct ComponentMapper      { typedef Rgb<byte> type; };
	template<>                      struct ComponentMapper<1,0> { typedef Rgb<unsigned short> type; };
	template<>                      struct ComponentMapper<0,1> { typedef byte type; };
	template<>                      struct ComponentMapper<0,0> { typedef unsigned short type; };

	class pnm_writer;
	class Writer
	{
		public:
			Writer(std::ostream&, ImageRef size, const std::string& type, const std::map<std::string, Parameter<> >& p);
			~Writer();

			//void write_raw_pixel_line(const bool*);
			void write_raw_pixel_line(const unsigned char*);
			void write_raw_pixel_line(const unsigned short*);
			void write_raw_pixel_line(const Rgb<unsigned char>*);
			void write_raw_pixel_line(const Rgb<unsigned short>*);

			template<class Incoming> struct Outgoing
			{		 
				typedef typename Pixel::Component<Incoming>::type Element;
				typedef typename ComponentMapper<Pixel::is_Rgb<Incoming>::value,
												 std::numeric_limits<Element>::is_integer &&
												 std::numeric_limits<Element>::digits <= 8>::type type;
			};		
			static const int top_row_first=1;

		private:
			std::auto_ptr<pnm_writer> p;
	};


  }
}
#endif
