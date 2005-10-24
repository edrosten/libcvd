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
//-*- c++ -*-
#ifndef CVD_INC_SE3_H
#define CVD_INC_SE3_H

#include <TooN/se3.h>

namespace CVD {

/// Class to represent a three-dimensional Euclidean transformation (a rotation and a translation). 
/// This can be represented by a member of the Special Euclidean Lie group SE3. These can be parameterised
/// six numbers (in the space of the Lie Algebra). In this class, the first three parameters are a
/// translation vector while the second three are a rotation vector, whose direction is the axis of rotation
/// and length the amount of rotation (in radians), as for SO3
///
/// See the TooN documentation
/// @ingroup gLinAlg
	
	using TooN::SE3;

}

#endif
