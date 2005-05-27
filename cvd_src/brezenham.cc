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
#include <cvd/brezenham.h>
#include <math.h>

using namespace TooN;

namespace CVD {

Brezenham::Brezenham(Vector<2> dir){
  if(fabs(dir[0]) > fabs(dir[1])){
    step1.y=0;
    if(dir[0] > 0){
      step1.x=1;
      val2=dir[0];
    } else {
      step1.x=-1;
      val2=-dir[0];
    }
    step2.x=0;
    if(dir[1] > 0){
      step2.y =1;
      val1=dir[1];
    } else {
      step2.y=-1;
      val1=-dir[1];
    }
  } else {
    step1.x=0;
    if(dir[1] > 0){
      step1.y=1;
      val2=dir[1];
    } else {
      step1.y=-1;
      val2=-dir[1];
    }
    step2.y=0;
    if(dir[0] > 0){
      step2.x =1;
      val1=dir[0];
    } else {
      step2.x=-1;
      val1=-dir[0];
    }
  }
  residual = val2/2;
}


ImageRef Brezenham::step(){
  if (residual >= val2){
    residual -= val2;
    return step2;
  }
  residual += val1;
  return step1;
}

Brezenham8::Brezenham8(Vector<2> dir){
  if(fabs(dir[0]) > fabs(dir[1])){
    step1.y=0;
    if(dir[0] > 0){
      step1.x=1;
      val2=dir[0];
    } else {
      step1.x=-1;
      val2=-dir[0];
    }
    step2.x=step1.x;
    if(dir[1] > 0){
      step2.y =1;
      val1=dir[1];
    } else {
      step2.y=-1;
      val1=-dir[1];
    }
    my_sideways.x=0;
    my_sideways.y=1;
  } else {
    step1.x=0;
    if(dir[1] > 0){
      step1.y=1;
      val2=dir[1];
    } else {
      step1.y=-1;
      val2=-dir[1];
    }
    step2.y=step1.y;
    if(dir[0] > 0){
      step2.x =1;
      val1=dir[0];
    } else {
      step2.x=-1;
      val1=-dir[0];
    }
    my_sideways.x=1;
    my_sideways.y=0;
  }
  residual = val2/2;
}


ImageRef Brezenham8::step(){
  residual += val1;
  if (residual >= val2){
    residual -= val2;
    return step2;
  }
  return step1;
}




} // namespace CVD

