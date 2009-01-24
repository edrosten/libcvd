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

	class WritePimpl;


	class writer
	{
		public:
			writer(std::ostream&, ImageRef size, const std::string& type);
			~writer();

			void write_raw_pixel_line(const byte*);
			void write_raw_pixel_line(const Rgb<byte>*);

			template<class Incoming> struct Outgoing
			{		
				typedef byte type;
			};		

		protected:
			std::auto_ptr<WritePimpl> t; 
	};

	template<class C> struct writer::Outgoing<Rgb<C> > 
	{
		typedef Rgb<byte> type;
	};


	template<class C> struct writer::Outgoing<Rgba<C> > 
	{
		typedef Rgb<byte> type;
	};

	template<> struct writer::Outgoing<Rgb8> 
	{
		typedef Rgb<byte> type;
	};

		
	class eps_writer
	{
		public:
			eps_writer(std::ostream&, ImageRef size, const std::string& type);
			~eps_writer();

			void write_raw_pixel_line(const byte*);
			void write_raw_pixel_line(const Rgb<byte>*);

			template<class Incoming> struct Outgoing
			{		
				typedef typename writer::Outgoing<Incoming>::type type;
			};		

		protected:
			std::auto_ptr<WritePimpl> t; 
	};

}
}
#endif

