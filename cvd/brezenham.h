#ifndef __BREZENHAM_H
#define __BREZENHAM_H

#include <numerics.h>
#include <cvd/image_ref.h>

namespace CVD {

class Brezenham {
 public:
  Brezenham(Vector<2> dir);

  ImageRef step();

 private:
  double residual;
  double val1;
  double val2;
  ImageRef step1;
  ImageRef step2;
};

class Brezenham8 {
 public:
  Brezenham8(Vector<2> dir);

  ImageRef step();

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
