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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef CVD_VECTOR_IMAGE_REF_H
#define CVD_VECTOR_IMAGE_REF_H

#include <numerics.h>
#include <cvd/image_ref.h>

namespace CVD
{
	/// Convert an image co-ordinate into a Vector
	/// @param ir The ImageRef to convert
	/// @ingroup gImage
	inline Vector<2> vec(const ImageRef& ir)
	{
		Vector<2> r;
		r[0] = ir.x;
		r[1] = ir.y;
		return r;
	}

	/// Convert a Vector into an image co-ordinate. Numbers are truncated, not rounded
	/// @param v The Vector to convert
	/// @ingroup gImage
	inline ImageRef ir(const Vector<2>& v)
	{
		return ImageRef((int)v[0], (int)v[1]);
	}
}


#endif
