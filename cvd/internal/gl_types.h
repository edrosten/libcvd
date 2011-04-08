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
// Pulled out of gl_helpers to make documentation neater
// Paul Smith 20/4/05

#ifndef CVD_GL_TYPES_H
#define CVD_GL_TYPES_H

#include <cvd/internal/gles1_types.h>

namespace CVD
{
	
	namespace gl
	{
		template<class C> struct data;

		//Scalar types
		template<> struct data<unsigned int>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<int>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_INT;
		};

		template<> struct data<double>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_DOUBLE;
		};

		//Rgb<*> types
	
		template<> struct data<Rgb<unsigned int> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<Rgb<int> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_INT;
		};

		template<> struct data<Rgb<double> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_DOUBLE;
		};

		//Rgba<*> types

		template<> struct data<Rgba<unsigned int> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<Rgba<int> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_INT;
		};

		template<> struct data<Rgba<double> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_DOUBLE;
		};

		//La<*> types

		template<> struct data<La<unsigned int> >
		{
			static const int format=GL_LUMINANCE_ALPHA;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<La<int> >
		{
			static const int format=GL_LUMINANCE_ALPHA;
			static const int type  =GL_INT;
		};

		template<> struct data<La<double> >
		{
			static const int format=GL_LUMINANCE_ALPHA;
			static const int type  =GL_DOUBLE;
		};

	};

};

#endif
