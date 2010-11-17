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
#ifndef CVD_VECTOR_IMAGE_REF_H
#define CVD_VECTOR_IMAGE_REF_H

#include <TooN/TooN.h>
#include <cvd/image_ref.h>

namespace CVD
{
	/// Convert an image co-ordinate into a Vector
	/// @param ir The ImageRef to convert
	/// @ingroup gImage
	inline TooN::Vector<2> vec(const ImageRef& ir)
	{
		TooN::Vector<2> r;
		r[0] = ir.x;
		r[1] = ir.y;
		return r;
	}

	/// Convert a Vector into an image co-ordinate. Numbers are truncated, not rounded
	/// @param v The Vector to convert
	/// @ingroup gImage
	inline ImageRef ir(const TooN::Vector<2>& v)
	{
		return ImageRef((int)v[0], (int)v[1]);
	}

	/// Convert a Vector into an image co-ordinate. Numbers are rounded
	/// @param v The Vector to convert
	/// @ingroup gImage
	inline ImageRef ir_rounded(const TooN::Vector<2>& v)
	{
		return ImageRef(
      static_cast<int>(v[0] > 0.0 ? v[0] + 0.5 : v[0] - 0.5),
      static_cast<int>(v[1] > 0.0 ? v[1] + 0.5 : v[1] - 0.5));
	}

	/// Rescale an ImageRef by a scaling factor
	/// @param v The Vector to convert
	/// @ingroup gImage
	inline ImageRef ir_rescale_rounded(const ImageRef &v, double rescaling_factor)
	{
	  return ir_rounded(vec(v) * rescaling_factor);
	}

}


#endif
