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
#ifndef CVD_IMAGE_CONVERT_FWD_H
#define CVD_IMAGE_CONVERT_FWD_H

#include <cvd/image.h>

namespace CVD
{
	//Forward declarations for convert_image
	template<class D, class C, class Conv> 	Image<D> convert_image(const BasicImage<C>& from, Conv& cv);
	template<class D, class C> Image<D> convert_image(const BasicImage<C>& from);
}

#endif
