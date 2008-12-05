/*                       
	This file is part of the CVD Library.

	Copyright (C) 2007 The Authors

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
//-*- c++ -*-
#ifndef CVD_INC_CONNECTED_COMPONENTS_H
#define CVD_CONNECTED_COMPONENTS_SO2_H

#include <cvd/image_ref.h>
#include <vector>

namespace CVD {

	///Find the connected components of the input, using 4-way floodfill.
	///This is implemented as the graph based algorithm. There is no restriction
	///on the input except that positions can not be INT_MIN or INT_MAX.
	///
	///The pixels in the resulting segments are not sorted.
	///@param v List of pixel positions
	///@param r List of segments.
	///@ingroup gVision
	void connected_components(const std::vector<ImageRef>& v, std::vector<std::vector<ImageRef> >& r);
}

#endif
