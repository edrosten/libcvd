//-*- c++ -*-

#ifndef __CAMERA_H
#define __CAMERA_H

#include <cmath>
#include <numerics.h>
#include <numhelpers.h>

template<class T>
inline T SAT(T x){return (x<-1.0/3?-1e9:x);}

namespace Camera {

  // Linear camera with zero skew
  class Linear {
  public:
    // The number of parameters in the camera
    static const int num_parameters=4;
    
    // load (save) parameters to (from) a stream
    inline void load(std::istream& is);
    inline void save(std::ostream& os);

    // project and unproject operations
    inline Vector<2> linearproject(const Vector<2>& camframe, double scale=1); // fast linear projection for working out what's there
    inline Vector<2> project(const Vector<2>& camframe); // project from euclidean camera frame to image plane
    inline Vector<2> unproject(const Vector<2>& imframe); // inverse operation
    
    // get the derivative of image frame wrt camera frame at the last computed projection
    // in the form (d im1/d cam1, d im1/d cam2)
    //             (d im2/d cam1, d im2/d cam2)
    inline Matrix<2,2> get_derivative();

    // get the motion of a point with respect to each of the internal camera parameters
    inline Matrix<num_parameters,2> get_parameter_derivs();

    // same but gets component of motion in direction provided
    inline Vector<num_parameters> get_parameter_derivs(const Vector<2>& direction);

    // update the internal camera parameters
    inline void update(const Vector<num_parameters>& updates);

    inline Vector<num_parameters>& get_parameters() {return my_camera_parameters;}

  private:
    Vector<num_parameters> my_camera_parameters; // f_u, f_v, u_0, v_0
    Vector<2> my_last_camframe;
  };


  class Cubic {
  public:
    // The number of parameters in the camera
    static const int num_parameters=5;
    
    // load (save) parameters to (from) a stream
    inline void load(std::istream& is);
    inline void save(std::ostream& os);

    // project and unproject operations
    inline Vector<2> linearproject(const Vector<2>& camframe, double scale=1); // fast linear projection for working out what's there
    inline Vector<2> project(const Vector<2>& camframe); // project from euclidean camera frame to image plane
    inline Vector<2> unproject(const Vector<2>& imframe); // inverse operation
    
    // get the derivative of image frame wrt camera frame at the last computed projection
    // in the form (d im1/d cam1, d im1/d cam2)
    //             (d im2/d cam1, d im2/d cam2)
    inline Matrix<2,2> get_derivative();

    // get the motion of a point with respect to each of the internal camera parameters
    inline Matrix<num_parameters,2> get_parameter_derivs();

    // same but gets component of motion in direction provided
    inline Vector<num_parameters> get_parameter_derivs(const Vector<2>& direction);

    // update the internal camera parameters
    inline void update(const Vector<num_parameters>& updates);

    inline Vector<num_parameters>& get_parameters() {return my_camera_parameters;}

  private:
    Vector<num_parameters> my_camera_parameters; // f_u, f_v, u_0, v_0
    Vector<2> my_last_camframe;
  };


  class Quintic {
  public:
    // The number of parameters in the camera
    static const int num_parameters=6;
    
    // load (save) parameters to (from) a stream
    inline void load(std::istream& is);
    inline void save(std::ostream& os);

    // project and unproject operations
    inline Vector<2> linearproject(const Vector<2>& camframe, double scale=1); // fast linear projection for working out what's there
    inline Vector<2> project(const Vector<2>& camframe); // project from euclidean camera frame to image plane
    inline Vector<2> unproject(const Vector<2>& imframe); // inverse operation
    
    // get the derivative of image frame wrt camera frame at the last computed projection
    // in the form (d im1/d cam1, d im1/d cam2)
    //             (d im2/d cam1, d im2/d cam2)
    inline Matrix<2,2> get_derivative();

    // get the motion of a point with respect to each of the internal camera parameters
    inline Matrix<num_parameters,2> get_parameter_derivs();

    // same but gets component of motion in direction provided
    inline Vector<num_parameters> get_parameter_derivs(const Vector<2>& direction);

    // update the internal camera parameters
    inline void update(const Vector<num_parameters>& updates);

    inline Vector<num_parameters>& get_parameters() {return my_camera_parameters;}

  private:
    Vector<num_parameters> my_camera_parameters; // f_u, f_v, u_0, v_0
    Vector<2> my_last_camframe;

	inline double sat(double x)
	{
		double a;
		a = (-3*my_camera_parameters[4] - sqrt(9*my_camera_parameters[4]*my_camera_parameters[4] - 20 * my_camera_parameters[5]))/(10*my_camera_parameters[5]);

		if(x < a)
			return x;
		else
			return 1e9; //(inf)
	}
  };


}


/////////////////////////////////////
// Camera::Linear inline functions //
/////////////////////////////////////

void Camera::Linear::load(std::istream& is) {
  is >> my_camera_parameters;
}

void Camera::Linear::save(std::ostream& os){
  os << my_camera_parameters;
}

inline Vector<2> Camera::Linear::linearproject(const Vector<2>& camframe, double scale){
  return Vector<2>(scale * diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline Vector<2> Camera::Linear::project(const Vector<2>& camframe){
  my_last_camframe = camframe;
  return Vector<2>(diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline Vector<2> Camera::Linear::unproject(const Vector<2>& imframe){
  my_last_camframe[0] = (imframe[0]-my_camera_parameters[2])/my_camera_parameters[0];
  my_last_camframe[1] = (imframe[1]-my_camera_parameters[3])/my_camera_parameters[1];
  return my_last_camframe;
}

Matrix<2,2> Camera::Linear::get_derivative(){
  Matrix<2,2> result;
  result(0,0) = my_camera_parameters[0];
  result(1,1) = my_camera_parameters[1];
  result(0,1) = result(1,0) = 0;
  return result;
}

Matrix<Camera::Linear::num_parameters,2> Camera::Linear::get_parameter_derivs(){
  Matrix<num_parameters,2> result;
  result(0,0) = my_last_camframe[0];
  result(0,1) = 0;
  result(1,0) = 0;
  result(1,1) = my_last_camframe[1];
  result(2,0) = 1;
  result(2,1) = 0;
  result(3,0) = 0;
  result(3,1) = 1;
  return result;
}

Vector<Camera::Linear::num_parameters> Camera::Linear::get_parameter_derivs(const Vector<2>& direction){
  Vector<num_parameters> result;
  result[0] = my_last_camframe[0] * direction[0];
  result[1] = my_last_camframe[1] * direction[1];
  result[2] = direction[0];
  result[3] = direction[1];

  return result;
}

void Camera::Linear::update(const Vector<num_parameters>& updates){
  my_camera_parameters+=updates;
}


/////////////////////////////////////
// Camera::Cubic inline functions //
/////////////////////////////////////

void Camera::Cubic::load(std::istream& is) {
  is >> my_camera_parameters;
}

void Camera::Cubic::save(std::ostream& os){
  os << my_camera_parameters;
}

inline Vector<2> Camera::Cubic::linearproject(const Vector<2>& camframe, double scale){
  return Vector<2>(scale * diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline Vector<2> Camera::Cubic::project(const Vector<2>& camframe){
  my_last_camframe = camframe;
  Vector<2> mod_camframe = camframe * (1+SAT(my_camera_parameters[4]*(camframe*camframe)));
  return Vector<2>(diagmult(mod_camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline Vector<2> Camera::Cubic::unproject(const Vector<2>& imframe){
  Vector<2> mod_camframe;
  mod_camframe[0] = (imframe[0]-my_camera_parameters[2])/my_camera_parameters[0];
  mod_camframe[1] = (imframe[1]-my_camera_parameters[3])/my_camera_parameters[1];

  // first guess
  double scale = 1+my_camera_parameters[4]*(mod_camframe*mod_camframe);

  // 3 iterations of Newton-Rapheson
  for(int i=0; i<3; i++){
    double error = my_camera_parameters[4]*(mod_camframe*mod_camframe) - scale*scale*(scale-1);
    double deriv = (3*scale -2)*scale;
    scale += error/deriv;
  }  
  my_last_camframe = mod_camframe/scale;

  return my_last_camframe;
}

Matrix<2,2> Camera::Cubic::get_derivative(){
  Matrix<2,2> result;
  Identity(result,1+my_camera_parameters[4]*(my_last_camframe*my_last_camframe));
  result += (2*my_camera_parameters[4]*my_last_camframe.as_col()) * my_last_camframe.as_row();
  result[0] *= my_camera_parameters[0];
  result[1] *= my_camera_parameters[1];
  return result;
}

Matrix<Camera::Cubic::num_parameters,2> Camera::Cubic::get_parameter_derivs(){
  Vector<2> mod_camframe = my_last_camframe * (1+my_camera_parameters[4]*(my_last_camframe*my_last_camframe));
  Matrix<num_parameters,2> result;
  result(0,0) = mod_camframe[0]*my_camera_parameters[0];
  result(0,1) = 0;
  result(1,0) = 0;
  result(1,1) = mod_camframe[1]*my_camera_parameters[1];
  result(2,0) = 1*my_camera_parameters[0];
  result(2,1) = 0;
  result(3,0) = 0;
  result(3,1) = 1*my_camera_parameters[1];
  result[4] = diagmult(my_last_camframe,my_camera_parameters.slice<0,2>())*(my_last_camframe*my_last_camframe);
  return result;
}

Vector<Camera::Cubic::num_parameters> Camera::Cubic::get_parameter_derivs(const Vector<2>& direction){
  Vector<2> mod_camframe = my_last_camframe * (1+my_camera_parameters[4]*(my_last_camframe*my_last_camframe));
  Vector<num_parameters> result;
  result[0] = mod_camframe[0] * direction[0] *my_camera_parameters[0];
  result[1] = mod_camframe[1] * direction[1] *my_camera_parameters[1];
  result[2] = direction[0] *my_camera_parameters[0];
  result[3] = direction[1] *my_camera_parameters[1];
  result[4] = (diagmult(my_last_camframe,my_camera_parameters.slice<0,2>())*direction)*(my_last_camframe*my_last_camframe);
  return result;
}

void Camera::Cubic::update(const Vector<num_parameters>& updates){
  double fu=my_camera_parameters[0];
  double fv=my_camera_parameters[1];
  my_camera_parameters[0]+=fu*updates[0];
  my_camera_parameters[1]+=fv*updates[1];
  my_camera_parameters[2]+=fu*updates[2];
  my_camera_parameters[3]+=fv*updates[3];
  my_camera_parameters[4]+=updates[4];
  //my_camera_parameters+=updates;
}

/////////////////////////////////////
// Camera::Quintic inline functions //
/////////////////////////////////////

void Camera::Quintic::load(std::istream& is) {
  is >> my_camera_parameters;
}

void Camera::Quintic::save(std::ostream& os){
  os << my_camera_parameters;
}

inline Vector<2> Camera::Quintic::linearproject(const Vector<2>& camframe, double scale){
  return Vector<2>(scale * diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline Vector<2> Camera::Quintic::project(const Vector<2>& camframe){
  my_last_camframe = camframe;
  double sc = /*sat*/(camframe*camframe);
  Vector<2> mod_camframe = camframe * (1 + sc*(my_camera_parameters[4] + sc*my_camera_parameters[5]));
  return Vector<2>(diagmult(mod_camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline Vector<2> Camera::Quintic::unproject(const Vector<2>& imframe){
  Vector<2> mod_camframe;
  mod_camframe[0] = (imframe[0]-my_camera_parameters[2])/my_camera_parameters[0];
  mod_camframe[1] = (imframe[1]-my_camera_parameters[3])/my_camera_parameters[1];

  // first guess
  double scale = mod_camframe*mod_camframe;

  // 3 iterations of Newton-Rapheson
  for(int i=0; i<3; i++){
    double temp=1+scale*(my_camera_parameters[4]+my_camera_parameters[5]*scale);
    double error = mod_camframe*mod_camframe - scale*temp*temp;
    double deriv = temp*(temp+2*scale*(my_camera_parameters[4]+2*my_camera_parameters[5]*scale));
    scale += error/deriv;
  }  
  my_last_camframe = mod_camframe/(1+scale*(my_camera_parameters[4]+my_camera_parameters[5]*scale));

  //std::cout<<"Done inverse on "<<imframe<<" - when reprojected get "<<project(my_last_camframe)<<std::endl;

  return my_last_camframe;
}

Matrix<2,2> Camera::Quintic::get_derivative(){
  Matrix<2,2> result;
  double temp1=my_last_camframe*my_last_camframe;
  double temp2=my_camera_parameters[5]*temp1;
  Identity(result,1+temp1*(my_camera_parameters[4]+temp2));
  result += (2*(my_camera_parameters[4]+2*temp2)*my_last_camframe.as_col()) * my_last_camframe.as_row();
  result[0] *= my_camera_parameters[0];
  result[1] *= my_camera_parameters[1];
  return result;
}

Matrix<Camera::Quintic::num_parameters,2> Camera::Quintic::get_parameter_derivs(){
  Matrix<num_parameters,2> result;
  double r2 = my_last_camframe * my_last_camframe;
  double r4 = r2 * r2;
  Vector<2> mod_camframe = my_last_camframe * (1+ r2 * (my_camera_parameters[4] + r2 * my_camera_parameters[5]));

  double fu = my_camera_parameters[0];
  double fv = my_camera_parameters[1];

  result(0,0) = mod_camframe[0] * fu;
  result(1,0) = 0;
  result(2,0) = 1*fu;
  result(3,0) = 0;
  result(4,0) = my_camera_parameters[0]*my_last_camframe[0]*r2;
  result(5,0) = my_camera_parameters[0]*my_last_camframe[0]*r4;

  result(0,1) = 0;
  result(1,1) = mod_camframe[1]*fv;
  result(2,1) = 0;
  result(3,1) = 1*fv;
  result(4,1) = my_camera_parameters[1]*my_last_camframe[1]*r2;
  result(5,1) = my_camera_parameters[1]*my_last_camframe[1]*r4;
  
  //cout<<"Finish me!\n";					     
  return result;
}

Vector<Camera::Quintic::num_parameters> Camera::Quintic::get_parameter_derivs(const Vector<2>& direction){
  //Vector<num_parameters> result;
  //cout<<"Finish me!\n";					     
  //FIXME improve this somewhat
  return get_parameter_derivs() * direction;
}

void Camera::Quintic::update(const Vector<num_parameters>& updates){
  double fu = my_camera_parameters[0];
  double fv = my_camera_parameters[1];

  my_camera_parameters[0]+=updates[0]*fu;
  my_camera_parameters[1]+=updates[1]*fv;
  my_camera_parameters[2]+=updates[2]*fu;
  my_camera_parameters[3]+=updates[3]*fv;
  my_camera_parameters[4]+=updates[4];
  my_camera_parameters[5]+=updates[5];
}


#endif
