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
#ifndef __BREZENHAM_H
#define __BREZENHAM_H

#include <TooN/TooN.h>
#include <cvd/image_ref.h>

namespace CVD {


/// Class to implement the Bresenham line-drawing algorithm.
/// This object does not draw directly into an image, it simply outputs the set
/// of image co-ordinates that should be visited to draw a line in a certain
/// direction. Pixels are generated in a 4-connected sense (i.e. there are 
/// no diagonal steps--each step is either horizontal or vertical)
/// details of the algorithm. See also Brezenham8.
/// @ingroup gGraphics
class Brezenham {
 public:
 /// Construct a line-drawing object
 /// @param dir The (x,y) direction of the line
  Brezenham(TooN::Vector<2> dir);

  /// Returns the next image co-ordinate along the line
  ImageRef step();

 private:
  double residual;
  double val1;
  double val2;
  ImageRef step1;
  ImageRef step2;
};


/// Class to implement the Bresenham line-drawing algorithm. 
/// This object does not draw directly into an image, it simply outputs the set
/// of image co-ordinates that should be visited to draw a line in a certain
/// direction. Pixels are generated in a 8-connected sense (i.e. diagonal steps
/// are possible). See also Brezenham.
/// @ingroup gGraphics
class Brezenham8 {
 public:
  /// Construct a line-drawing object
  /// @param dir The (x,y) direction of the line
  Brezenham8(TooN::Vector<2> dir);

  /// Returns the next image co-ordinate along the line
  ImageRef step();

  /// Which compass position most orthogonal to the line's direction?
  /// At each pixel given by 
  /// step() you are guaranteed to be able to walk in the direction given
  /// by sideways and not walk on the line.
  ImageRef sideways() {return my_sideways;}

 private:
  double residual;
  double val1;
  double val2;
  ImageRef step1;
  ImageRef step2;
  ImageRef my_sideways;
};

}

#endif
