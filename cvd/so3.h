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
#ifndef CVD_INC_SO3_H
#define CVD_INC_SO3_H

#include <TooN/so3.h>

namespace CVD {

/// Class to represent a three-dimensional rotation matrix. Three-dimensional rotation
/// matrices are members of the Special Orthogonal Lie group SO3. This group can be parameterised
/// three numbers (a vector in the space of the Lie Algebra). In this class, the three parameters are the
/// finite rotation vector, i.e. a three-dimensional vector whose direction is the axis of rotation
/// and whose length is the angle of rotation in radians. Exponentiating this vector gives the matrix,
/// and the logarithm of the matrix gives this vector.
///
/// See the TooN documentation
/// @ingroup gLinAlg 
	
	using TooN::SO3;

}

#endif
