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
#ifndef CVD_INCLUDE_INTERNAL_IO_TEXT_H
#define CVD_INCLUDE_INTERNAL_IO_TEXT_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cvd/image.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
namespace TEXT
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

			void get_raw_pixel_line(double*);

			std::string datatype();
			std::string name();


			typedef TypeList<double,  Head> Types;
		
		private:
			std::auto_ptr<ReadPimpl> t; 
	};


	////////////////////////////////////////////////////////////////////////////////
	//
	// TEXT writing
	//
	class WritePimpl;

	class writer
	{
		public:
			writer(std::ostream&, ImageRef size, const std::string& type, const std::map<std::string, Parameter<> >& p);
			~writer();

			void write_raw_pixel_line(const double*);
			void write_raw_pixel_line(const float*);

			template<class Incoming> struct Outgoing
			{		
				typedef double type;
			};		

			static const int top_row_first=1;
		private:
			std::auto_ptr<WritePimpl> t; 
	};
	
}
}
#endif
