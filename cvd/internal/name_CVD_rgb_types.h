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
#ifndef PNM_NAME_CVD_RGB_TYPES_H
#define PNM_NAME_CVD_RGB_TYPES_H

#include <string>
#include <cvd/internal/name_builtin_types.h>

namespace CVD
{
namespace PNM
{

	template<> struct  type_name<CVD::Rgb8>{static const std::string name(){return "CVD::Rgb8";}};
	template<class O> struct type_name<CVD::Rgb<O> >
	{
		static const std::string name(){return "CVD::Rgb<" + type_name<O>::name() + ">";}
	};
	template<class O> struct type_name<CVD::Rgba<O> >
	{
		static const std::string name(){return "CVD::Rgba<" + type_name<O>::name() + ">";}
	};
}
}

#endif

