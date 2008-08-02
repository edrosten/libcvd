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
#ifndef PNM_TIFF
#define PNM_TIFF

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cvd/image.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
namespace TIFF
{

	using CVD::Internal::TypeList;
	using CVD::Internal::Head;

	class TIFFPimpl;

	class tiff_reader
	{
		public:
			tiff_reader(std::istream&);
			~tiff_reader();

			ImageRef size();

			void get_raw_pixel_lines(bool*, unsigned long nlines);
			void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
			void get_raw_pixel_lines(unsigned short*, unsigned long nlines);
			void get_raw_pixel_lines(float*, unsigned long nlines);
			void get_raw_pixel_lines(double*, unsigned long nlines);

			void get_raw_pixel_lines(Rgb<unsigned char>*, unsigned long nlines);
			void get_raw_pixel_lines(Rgb<unsigned short>*, unsigned long nlines);
			void get_raw_pixel_lines(Rgb<float>*, unsigned long nlines);
			void get_raw_pixel_lines(Rgb<double>*, unsigned long nlines);

			void get_raw_pixel_lines(Rgba<unsigned char>*, unsigned long nlines);
			void get_raw_pixel_lines(Rgba<unsigned short>*, unsigned long nlines);
			void get_raw_pixel_lines(Rgba<float>*, unsigned long nlines);
			void get_raw_pixel_lines(Rgba<double>*, unsigned long nlines);

			std::string datatype();
			std::string name();


			typedef TypeList<bool, 
					TypeList<byte, 
					TypeList<unsigned short,
					TypeList<float,
					TypeList<double,
					TypeList<Rgb<byte>, 
					TypeList<Rgb<unsigned short>, 
					TypeList<Rgb<float>, 
					TypeList<Rgb<double>, 
					TypeList<Rgba<byte>, 
					TypeList<Rgba<unsigned short>, 
					TypeList<Rgba<float>, 
					TypeList<Rgba<double>, 
					                      Head> > > > > > > > > > > > > Types;
		
		private:
			TIFFPimpl* t; 
	};

	
	template <class T> void readTIFF(BasicImage<T>& im, std::istream& in)
	{
		CVD::Internal::readImage<T, tiff_reader>(im, in);
	}

	template <class T> void readTIFF(Image<T>& im, std::istream& in)
	{
		CVD::Internal::readImage<T, tiff_reader>(im, in);
	}

}
}
#endif
