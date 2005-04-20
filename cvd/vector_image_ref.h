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
