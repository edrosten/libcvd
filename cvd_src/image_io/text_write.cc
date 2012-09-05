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
#include "cvd/internal/io/text.h"
#include "cvd/image_io.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <iomanip>

using namespace CVD;
using namespace CVD::TEXT;
using namespace CVD::Exceptions::Image_IO;
using namespace std;

class CVD::TEXT::WritePimpl
{
	public:
		WritePimpl(ostream&oo, ImageRef size, const string& t)
		:o(oo),my_size(size),type(t),row(0)
		{
			if(t != "double" && t != "float")
				throw UnsupportedImageSubType("TEXT", t);
		}

		~WritePimpl()
		{
		}

		void write_raw_pixel_line(const double* d)
		{
			if(type != "double")
				throw WriteTypeMismatch(type, "double");

			//Do some sanity checking
			if(row >= (unsigned long)my_size.y)
				throw InternalLibraryError("CVD", "Write past end of image.");
			o <<scientific << setw(24) << setprecision(16);

			copy(d, d + my_size.x-1, ostream_iterator<double>(o, " "));
			o << d[my_size.x-1] << endl;
			row++;
		}

		void write_raw_pixel_line(const float* d)
		{
			if(type != "float")
				throw WriteTypeMismatch(type, "float");

			//Do some sanity checking
			if(row >= (unsigned long)my_size.y)
				throw InternalLibraryError("CVD", "Write past end of image.");
			o <<scientific << setw(15) << setprecision(8);

			copy(d, d + my_size.x-1, ostream_iterator<float>(o, " "));
			o << d[my_size.x-1] << endl;
			row++;
		}

	private:
		ostream& o;
		ImageRef my_size;
		string   type;
		unsigned long row;
};

TEXT::writer::writer(ostream& o, ImageRef size, const string& type, const std::map<std::string, Parameter<> >&)
:t(new WritePimpl(o, size, type))
{}

TEXT::writer::~writer()
{}

void TEXT::writer::write_raw_pixel_line(const double* d)
{
	t->write_raw_pixel_line(d);
}


void TEXT::writer::write_raw_pixel_line(const float* d)
{
	t->write_raw_pixel_line(d);
}



















