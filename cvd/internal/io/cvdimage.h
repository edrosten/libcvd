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
#ifndef PNM_CVDIMAGE_H
#define PNM_CVDIMAGE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/colourspaces.h>
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
namespace CVDimage
{

	using CVD::Internal::TypeList;
	using CVD::Internal::Head;


	class ReadPimpl;
	class reader
	{
		public:
			reader(std::istream&);
			~reader();

			ImageRef size();
			bool top_row_first();

			void get_raw_pixel_line(unsigned char*);
			void get_raw_pixel_line(bayer_bggr*);
			void get_raw_pixel_line(bayer_rggb*);
			void get_raw_pixel_line(bayer_grbg*);
			void get_raw_pixel_line(bayer_gbrg*);
			void get_raw_pixel_line(Rgb<unsigned char>*);
			void get_raw_pixel_line(Rgba<unsigned char>*);

			std::string datatype();
			std::string name();


			typedef TypeList<byte,
					TypeList<bayer_bggr, 
					TypeList<bayer_rggb, 
					TypeList<bayer_grbg, 
					TypeList<bayer_gbrg, 
					TypeList<Rgb<byte>, 
					TypeList<Rgba<byte>, 
					Head> > > > > > > Types;
		
		private:
			std::auto_ptr<ReadPimpl> t; 
	};


	class WritePimpl;

	class writer
	{
		public:
			writer(std::ostream&, ImageRef size, const std::string& type, const std::map<std::string, Parameter<> >& p);
			~writer();

			void write_raw_pixel_line(const byte*);
			void write_raw_pixel_line(const bayer_bggr*);
			void write_raw_pixel_line(const bayer_rggb*);
			void write_raw_pixel_line(const bayer_grbg*);
			void write_raw_pixel_line(const bayer_gbrg*);
			void write_raw_pixel_line(const Rgb<byte>*);
			void write_raw_pixel_line(const Rgba<byte>*);

			template<class Incoming> struct Outgoing
			{		
				typedef byte type;
			};		

			static const int top_row_first=1;
		protected:
			std::auto_ptr<WritePimpl> t; 
	};

	template <> struct writer::Outgoing<bayer_bggr> 
	{
		typedef bayer_bggr type;
	};

	template <> struct writer::Outgoing<bayer_rggb> 
	{
		typedef bayer_rggb type;
	};

	template <> struct writer::Outgoing<bayer_grbg> 
	{
		typedef bayer_grbg type;
	};

	template <> struct writer::Outgoing<bayer_gbrg> 
	{
		typedef bayer_gbrg type;
	};

	template<class C> struct writer::Outgoing<Rgb<C> > 
	{
		typedef Rgb<byte> type;
	};

	template<class C> struct writer::Outgoing<Rgba<C> > 
	{
		typedef Rgba<byte> type;
	};

	template<> struct writer::Outgoing<Rgb8> 
	{
		typedef Rgb<byte> type;
	};

}
}

#endif
