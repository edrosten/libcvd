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
#ifndef CVD_ABS_H
#define CVD_ABS_H

namespace CVD
{
	/// Local templated version of abs. Returns the absolute value of a variable.
	/// @param t The input parameter
	/// @ingroup gCPP
	template <class T> 
	inline T abs(T t) { return t<0 ? -t : t; }
	/// Fast instantiation for unsigned datatype which avoids a comparison
	/// @param b The input parameter
	/// @ingroup gCPP
	inline unsigned char abs(unsigned char b) { return b; }
	/// Fast instantiation for unsigned datatype which avoids a comparison
	/// @param u The input parameter
	/// @ingroup gCPP
	inline unsigned short abs(unsigned short u) { return u; }
	/// Fast instantiation for unsigned datatype which avoids a comparison
	/// @param u The input parameter
	/// @ingroup gCPP
	inline unsigned int abs(unsigned int u) { return u; }
	/// Fast instantiation for unsigned datatype which avoids a comparison
	/// @param u The input parameter
	/// @ingroup gCPP
	inline unsigned long abs(unsigned long u) { return u; }
}
#endif

