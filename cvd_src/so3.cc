#include "cvd/so3.h"
#include <math.h>
#include <numhelpers.h>

namespace CVD {

static double Identity[9] = {1,0,0,0,1,0,0,0,1};

SO3::SO3() :
  my_matrix(Identity)
{}

SO3& SO3::operator=(const SO3& rhs){
  my_matrix = rhs.my_matrix;
  coerce();
  return *this;
}

SO3& SO3::operator=(const Matrix<3>& rhs){
  my_matrix = rhs;
  coerce();
  return *this;
}

void SO3::coerce(){
  normalize(my_matrix[0]);
  my_matrix[1] -= my_matrix[0] * (my_matrix[0]*my_matrix[1]);
  normalize(my_matrix[1]);
  my_matrix[2] -= my_matrix[0] * (my_matrix[0]*my_matrix[2]);
  my_matrix[2] -= my_matrix[1] * (my_matrix[1]*my_matrix[2]);
  normalize(my_matrix[2]);
}


SO3 SO3::exp(const double* vect){
  SO3 result;
  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      result.my_matrix[i][j] = 0;
    }
  }

  double theta = sqrt(vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2]);
  
  double stot = 1;
  double shtot = 0.5;

  double ct=cos(theta);

  if(theta > 0.001) {
    stot = sin(theta)/theta;
    shtot = sin(theta/2)/theta;
  }

  result.my_matrix[0][1] -= stot*vect[2];
  result.my_matrix[0][2] += stot*vect[1];
  result.my_matrix[1][0] += stot*vect[2];
  result.my_matrix[1][2] -= stot*vect[0];
  result.my_matrix[2][0] -= stot*vect[1];
  result.my_matrix[2][1] += stot*vect[0];

  result.my_matrix[0][0] += ct;
  result.my_matrix[1][1] += ct;
  result.my_matrix[2][2] += ct;

  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      result.my_matrix[i][j] += 2*shtot*shtot*vect[i]*vect[j];
    }
  }
  return result;
}


Vector<3> SO3::ln() const{
  Vector<3> result;

  double trace = my_matrix[0][0] + my_matrix[1][1] + my_matrix[2][2];

  result[0] = (my_matrix[2][1]-my_matrix[1][2])/2;
  result[1] = (my_matrix[0][2]-my_matrix[2][0])/2;
  result[2] = (my_matrix[1][0]-my_matrix[0][1])/2;

  double sin_angle_abs = sqrt(result*result);

  // FIX PAS 10/5/02 added the min since was occasionally giving a fraction over 1.0
  if(sin_angle_abs > 1.0)
     sin_angle_abs = 1.0;
  // END FIX

  double tost=1;
  if (sin_angle_abs > 0.00001){
    double angle = asin(sin_angle_abs);
    if(trace < 1) {
      angle = M_PI - angle;
    }

    tost = angle / sin_angle_abs;
  }
  
  
  result *=tost;

  return result;
}

SO3 SO3::inverse() const{
  SO3 result;
  result.my_matrix[0][0] = my_matrix[0][0];
  result.my_matrix[0][1] = my_matrix[1][0];
  result.my_matrix[0][2] = my_matrix[2][0];
  result.my_matrix[1][0] = my_matrix[0][1];
  result.my_matrix[1][1] = my_matrix[1][1];
  result.my_matrix[1][2] = my_matrix[2][1];
  result.my_matrix[2][0] = my_matrix[0][2];
  result.my_matrix[2][1] = my_matrix[1][2];
  result.my_matrix[2][2] = my_matrix[2][2];
  return result;
}


// SO3& SO3::operator *=(const SO3& rhs){
//   *this = *this * rhs;
//   return *this;
// }

// SO3 SO3::operator *(const SO3& rhs)const{
//   SO3 result;
//   result.my_matrix = my_matrix * rhs.my_matrix;
//   return result;
// }

Vector<3> SO3::generator_field(int i, Vector<3> pos){
  Vector<3> result;
  result[i]=0;
  result[(i+1)%3] = - pos[(i+2)%3];
  result[(i+2)%3] = pos[(i+1)%3];
  return result;
}

Vector<3> SO3::adjoint(Vector<3> vect)const {
  return my_matrix * vect;
}


} // namespace CVD
