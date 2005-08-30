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
#include "cvd/se3.h"
#include "cvd/so3.h"

#include <iostream>
#include <math.h>

using namespace TooN;

namespace CVD {

static double zero[3]={0,0,0};

SE3::SE3() :
  my_translation(zero)
{}


SE3 SE3::exp(const Vector<6>& vect){
  SE3 result;
  
  result.my_rotation = SO3::exp(vect.slice<3,3>());  

  double theta = sqrt(vect[3]*vect[3] + vect[4]*vect[4] + vect[5]*vect[5]);
  
  double shtot = 0.5;

  if(theta > 0.00001) {  // accurate up to 10^-10
    shtot = sin(theta/2)/theta;
  }


  // now do the rotation
  SO3 halfrotator = SO3::exp(vect.slice<3,3>()/2);

  Vector<3> trans(vect.slice<0,3>());
  Vector<3> rot(vect.slice<3,3>());

  result.my_translation = halfrotator * trans * (2*shtot);

  if(theta > 0.001){
    result.my_translation += rot * ((trans * rot) * (1- 2*shtot) / (rot * rot));
  } else {
    // small angle approximation that considers first two terms in expansion of sin(theta/2)
    // accurate up to 10^-15
    result.my_translation += rot * ((trans * rot)/24);
  }

  return result;
}

Vector<6> SE3::ln() const{
  Vector<3> rot = my_rotation.ln();
  double theta = sqrt(rot*rot);
  double shtot = 0.5;

  if(theta > 0.00001) {
    shtot = sin(theta/2)/theta;
  }

  // now do the rotation
  Vector<3> halfrot = rot * -0.5;
  SO3 halfrotator = SO3::exp(halfrot);

  Vector<3> rottrans = halfrotator * my_translation;
  
  if(theta > 0.001){
    rottrans -= rot * ((my_translation * rot) * (1-2*shtot) / (rot*rot));
  } else {
    rottrans -= rot * ((my_translation * rot)/24);
  }

  rottrans /= (2 * shtot);

  Vector<6> result;
  result.slice<0,3>()=rottrans;
  result.slice<3,3>()=rot;
  return result;
}

SE3 SE3::inverse() const {
  SE3 result;
  result.my_rotation = my_rotation.inverse();
  result.my_translation = result.my_rotation * my_translation;
  result.my_translation *= -1;

  return result;
}

SE3 SE3::operator*(const SE3& rhs) const {
  SE3 result;
  result.my_rotation = my_rotation * rhs.my_rotation;
  result.my_translation = my_translation + (my_rotation * rhs.my_translation);
  return result;
}


Vector<4> SE3::generator_field(int i, Vector<4> pos){
  double result_d[]={0,0,0,0};
  Vector<4> result(result_d);
  if(i < 3){
    result[i]=pos[3];
    return result;
  }
  result[(i+1)%3] = - pos[(i+2)%3];
  result[(i+2)%3] = pos[(i+1)%3];
  return result;
}


SE3 operator*(const SO3& lhs, const SE3& rhs){
  SE3 result;
  result.my_rotation = lhs*rhs.my_rotation;
  result.my_translation = lhs*rhs.my_translation;
  return result;
}





} // namespace CVD
