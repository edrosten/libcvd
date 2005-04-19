#ifndef __BREZENHAM_H
#define __BREZENHAM_H

#include <numerics.h>
#include <cvd/image_ref.h>

namespace CVD {


/// Class to implement the Bresenham line-drawing algorithm.
/// This object does not draw directly into an image, it simply outputs the set
/// of image co-ordinates that should be visited to draw a line in a certain
/// direction. Pixels are generated in a 4-connected sense (i.e. a diagonal
/// step is possible). See 
/// http://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html for more
/// details of the algorithm. 
/// @ingroup gGraphics
class Brezenham {
 public:
 /// Construct a line-drawing object
 /// @param dir The (x,y) direction of the line
  Brezenham(Vector<2> dir);

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
/// direction. Pixels are generated in a 8-connected sense (i.e. each step is
/// either horizontal or vertical). See 
/// http://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html for more
/// details of the algorithm. 
/// @ingroup gGraphics
class Brezenham8 {
 public:
  /// Construct a line-drawing object
  /// @param dir The (x,y) direction of the line
  Brezenham8(Vector<2> dir);

  /// Returns the next image co-ordinate along the line
  ImageRef step();

  /// Which compass position most orthogonal to the line's direction?
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
