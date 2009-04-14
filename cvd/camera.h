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

#ifndef __CAMERA_H
#define __CAMERA_H

#include <cmath>
#include <TooN/TooN.h>
#include <TooN/helpers.h>

template<class T>
inline T SAT(T x){return (x<-1.0/3?-1e9:x);}

/// Classes which represent camera calibrations.
/// @ingroup gVision
namespace Camera {

  /// A linear camera with zero skew
  /// @ingroup gVision
  class Linear {
  public:
    /// The number of parameters in the camera
    static const int num_parameters=4;

	///Load parameters from a stream 
	///@param is The stream to use
    inline void load(std::istream& is); 
	/// Save parameters to a stream 
	///@param os The stream to use
    inline void save(std::ostream& os); 

    /// Fast linear projection for working out what's there
    inline TooN::Vector<2> linearproject(const TooN::Vector<2>& camframe, double scale=1) const;
	/// Project from Euclidean camera frame to image plane
    inline TooN::Vector<2> project(const TooN::Vector<2>& camframe) const;
	/// Project from image plane to a Euclidean camera
    inline TooN::Vector<2> unproject(const TooN::Vector<2>& imframe) const;
    
    /// Get the derivative of image frame wrt camera frame at the last computed projection
    /// in the form \f$ \begin{bmatrix} \frac{\partial \text{im1}}{\partial \text{cam1}} & \frac{\partial \text{im1}}{\partial \text{cam2}} \\ \frac{\partial \text{im2}}{\partial \text{cam1}} & \frac{\partial \text{im2}}{\partial \text{cam2}} \end{bmatrix} \f$
    inline TooN::Matrix<2,2> get_derivative() const;

    /// Get the motion of a point with respect to each of the internal camera parameters
    inline TooN::Matrix<num_parameters,2> get_parameter_derivs() const ;

	/// Get the component of the motion of a point in the direction provided 
	///	with respect to each of the internal camera parameters
	/// @param direction The (x,y) direction to use
    inline TooN::Vector<num_parameters> get_parameter_derivs(const TooN::Vector<2>& direction) const ;

	/// Update the internal camera parameters by adding the vector given
	/// @param updates Update vector in the format 
	/// \f$ \begin{pmatrix}\Delta f_u & \Delta f_v & \Delta u_0 & \Delta v_0 \end{pmatrix} \f$
    inline void update(const TooN::Vector<num_parameters>& updates);

	/// Returns the vector of camera parameters in the format
	/// \f$ \begin{pmatrix}f_u & f_v & u_0 & v_0 \end{pmatrix} \f$
    inline TooN::Vector<num_parameters>& get_parameters() {return my_camera_parameters;}
    inline const TooN::Vector<num_parameters>& get_parameters() const {return my_camera_parameters;}

  private:
    TooN::Vector<num_parameters> my_camera_parameters; 
    mutable TooN::Vector<2> my_last_camframe;
  };


  /// A camera with zero skew and cubic distortion
  /// @ingroup gVision
  class Cubic {
  public:
    /// The number of parameters in the camera
    static const int num_parameters=5;
    
	///Load parameters from a stream 
	///@param is The stream to use
    inline void load(std::istream& is);
	/// Save parameters to a stream 
	///@param os The stream to use
    inline void save(std::ostream& os);

    /// Fast linear projection for working out what's there
    inline TooN::Vector<2> linearproject(const TooN::Vector<2>& camframe, double scale=1); 
	/// Project from Euclidean camera frame to image plane
    inline TooN::Vector<2> project(const TooN::Vector<2>& camframe) const; 
	/// Project from image plane to a Euclidean camera
    inline TooN::Vector<2> unproject(const TooN::Vector<2>& imframe); 
    
    /// Get the derivative of image frame wrt camera frame at the last computed projection
    /// in the form \f$ \begin{bmatrix} \frac{\partial \text{im1}}{\partial \text{cam1}} & \frac{\partial \text{im1}}{\partial \text{cam2}} \\ \frac{\partial \text{im2}}{\partial \text{cam1}} & \frac{\partial \text{im2}}{\partial \text{cam2}} \end{bmatrix} \f$
    inline TooN::Matrix<2,2> get_derivative();

    /// Get the motion of a point with respect to each of the internal camera parameters
    inline TooN::Matrix<num_parameters,2> get_parameter_derivs() const ;

	/// Get the component of the motion of a point in the direction provided 
	///	with respect to each of the internal camera parameters
	/// @param direction The (x,y) direction to use
    inline TooN::Vector<num_parameters> get_parameter_derivs(const TooN::Vector<2>& direction) const ;

	/// Update the internal camera parameters by adding the vector given
	/// @param updates Update vector in the format 
	/// \f$ \begin{pmatrix}\Delta f_u & \Delta f_v & \Delta u_0 & \Delta v_0 & \Delta c\end{pmatrix} \f$
    inline void update(const TooN::Vector<num_parameters>& updates);

	/// Returns the vector of camera parameters in the format
	/// \f$ \begin{pmatrix}f_u & f_v & u_0 & v_0 & c\end{pmatrix} \f$
    inline TooN::Vector<num_parameters>& get_parameters() {return my_camera_parameters;}

  private:
    TooN::Vector<num_parameters> my_camera_parameters; // f_u, f_v, u_0, v_0
    mutable TooN::Vector<2> my_last_camframe;
  };


  /// A camera with zero skew and quintic distortion
  /// @ingroup gVision
  class Quintic {
  public:
    /// The number of parameters in the camera
    static const int num_parameters=6;
    
	///Load parameters from a stream 
	///@param is The stream to use
    inline void load(std::istream& is);
	/// Save parameters to a stream 
	///@param os The stream to use
    inline void save(std::ostream& os);

    /// Fast linear projection for working out what's there
    inline TooN::Vector<2> linearproject(const TooN::Vector<2>& camframe, double scale=1) const ;
	/// Project from Euclidean camera frame to image plane

	inline TooN::Vector<2> project_vector(const TooN::Vector<2>& x, const TooN::Vector<2>& d) const {
	    const double xsq = x*x;
	    const double& a = my_camera_parameters[4];
	    const double& b = my_camera_parameters[5];
	    return (2 * (a + 2*b*xsq) * (x*d) * TooN::diagmult(my_camera_parameters.slice<0,2>(), x) +
		    (1 + a*xsq + b*xsq*xsq)*TooN::diagmult(my_camera_parameters.slice<0,2>(), d));
	}

	inline TooN::Vector<2> project_vector(const TooN::Vector<2>& d) const {
	    return diagmult(my_camera_parameters.slice<0,2>(), d);
	}
	inline TooN::Vector<2> unproject_vector(const TooN::Vector<2>& d) const {
	    TooN::Vector<2> v;
	    v[0] = d[0]/my_camera_parameters[0];
	    v[1] = d[1]/my_camera_parameters[1];
	    return v;
	}
    inline TooN::Vector<2> project(const TooN::Vector<2>& camframe) const; 
    inline std::pair<TooN::Vector<2>, TooN::Matrix<2> > project(const TooN::Vector<2>& camframe, const TooN::Matrix<2>& R) const;

	/// Project from image plane to a Euclidean camera
    inline TooN::Vector<2> unproject(const TooN::Vector<2>& imframe) const;

    inline std::pair<TooN::Vector<2>, TooN::Matrix<2> > unproject(const TooN::Vector<2>& imframe, const TooN::Matrix<2>& R) const;
    
    /// Get the derivative of image frame wrt camera frame at the last computed projection
    /// in the form \f$ \begin{bmatrix} \frac{\partial \text{im1}}{\partial \text{cam1}} & \frac{\partial \text{im1}}{\partial \text{cam2}} \\ \frac{\partial \text{im2}}{\partial \text{cam1}} & \frac{\partial \text{im2}}{\partial \text{cam2}} \end{bmatrix} \f$
    inline TooN::Matrix<2,2> get_derivative() const;
    inline TooN::Matrix<2,2> get_derivative(const TooN::Vector<2>& x) const;

    inline TooN::Matrix<2,2> get_inv_derivative() const;
    inline TooN::Matrix<2,2> get_inv_derivative(const TooN::Vector<2>& x) const;

    /// Get the motion of a point with respect to each of the internal camera parameters
    inline TooN::Matrix<num_parameters,2> get_parameter_derivs() const ;

	/// Get the component of the motion of a point in the direction provided 
	///	with respect to each of the internal camera parameters
	/// @param direction The (x,y) direction to use
    inline TooN::Vector<num_parameters> get_parameter_derivs(const TooN::Vector<2>& direction) const ;

	/// Update the internal camera parameters by adding the vector given
	/// @param updates Update vector in the format 
	/// \f$ \begin{pmatrix}\Delta f_u & \Delta f_v & \Delta u_0 & \Delta v_0 & \Delta c & \Delta q\end{pmatrix} \f$
    inline void update(const TooN::Vector<num_parameters>& updates);

	/// Returns the vector of camera parameters in the format
	/// \f$ \begin{pmatrix}f_u & f_v & u_0 & v_0 & c & q\end{pmatrix} \f$
    inline const TooN::Vector<num_parameters>& get_parameters() const {return my_camera_parameters;}
	/// Returns the vector of camera parameters in the format
	/// \f$ \begin{pmatrix}f_u & f_v & u_0 & v_0 & c & q\end{pmatrix} \f$
    inline TooN::Vector<num_parameters>& get_parameters() {return my_camera_parameters;}

  private:
    TooN::Vector<num_parameters> my_camera_parameters; // f_u, f_v, u_0, v_0
    mutable TooN::Vector<2> my_last_camframe;

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

	///A Camera with zero skew and Harris distortion. The camera distortion model is as follows:
	///
	///\f$
	/// \hat{\rho} = \frac{\rho}{\sqrt{1 + \alpha \rho^2}}
	///\f$
	///
	///This camera has the advantage that inversion of the radial distortion is trivial,
	///and unproject has a unique, closed-form solution. However, the square root makes
	///this camera slower than some of the other models for many use cases.
	///@ingroup gVision
	class Harris{

		private:
			TooN::Vector<2> radial_distort(const TooN::Vector<2>& camframe) const
			{
				double r2 = camframe*camframe;
				return camframe / sqrt(1 + my_camera_parameters[4] * r2);
			}
			

		public:
			/// The number of parameters in the camera
			static const int num_parameters=5;
    
			///Load parameters from a stream 
			///@param is The stream to use
			inline void load(std::istream& is);

			/// Save parameters to a stream 
			///@param os The stream to use
			inline void save(std::ostream& os)
			{
				os << my_camera_parameters;
			}


			/// Fast linear projection for working out what's there
			inline TooN::Vector<2> linearproject(const TooN::Vector<2>& camframe, double scale=1) const
			{
				return TooN::Vector<2>(scale * diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
			}

			/// Project from Euclidean camera frame to image plane
			inline TooN::Vector<2> project(const TooN::Vector<2>& camframe) const 
			{
				my_last_camframe = camframe;
				return linearproject(radial_distort(camframe));
			}

			/// Project from image plane to a Euclidean camera
			inline TooN::Vector<2> unproject(const TooN::Vector<2>& imframe)
			{
				//Undo the focal length and optic axis.
				TooN::Vector<2> mod_camframe;
				mod_camframe[0] = (imframe[0]-my_camera_parameters[2])/my_camera_parameters[0];
				mod_camframe[1] = (imframe[1]-my_camera_parameters[3])/my_camera_parameters[1];
				double rprime2 = mod_camframe*mod_camframe;
				my_last_camframe =  mod_camframe / sqrt(1 - my_camera_parameters[4] * rprime2);
				return my_last_camframe;
			}
			
			/// Get the derivative of image frame wrt camera frame at the last computed projection
			/// in the form \f$ \begin{bmatrix} \frac{\partial \text{im1}}{\partial \text{cam1}} & \frac{\partial \text{im1}}{\partial \text{cam2}} \\ \frac{\partial \text{im2}}{\partial \text{cam1}} & \frac{\partial \text{im2}}{\partial \text{cam2}} \end{bmatrix} \f$
			inline TooN::Matrix<2,2> get_derivative() const
			{
				TooN::Matrix<2,2> J;

				double xc = my_last_camframe[0];
				double yc = my_last_camframe[1];

				double fu= my_camera_parameters[0];
				double fv= my_camera_parameters[1];
				double a = my_camera_parameters[4];

				double g = 1/sqrt(1 + a * (xc*xc + yc*yc));
				double g3= g*g*g;
				
				J[0][0] = fu * (g - 2 * a * xc*xc*g3);
				J[0][1] = -2 * fu * a * xc * yc * g3;
				J[1][0] = -2 * fv * a * xc * yc * g3;
				J[1][1] = fv * (g - 2 * a * yc*yc*g3);

				return J;
			}

			/// Get the motion of a point with respect to each of the internal camera parameters
			inline TooN::Matrix<num_parameters,2> get_parameter_derivs() const 
			{
				TooN::Vector<2> mod_camframe = radial_distort(my_last_camframe);

				TooN::Matrix<5, 2> result;

				double xc = my_last_camframe[0];
				double yc = my_last_camframe[1];
				double r2 = xc*xc + yc*yc;

				double fu= my_camera_parameters[0];
				double fv= my_camera_parameters[1];
				double a = my_camera_parameters[4];

				double g = 1/sqrt(1 + a * r2);
				double g3= g*g*g;

				//Derivatives of x_image:
				result[0][0] = mod_camframe[0];
				result[1][0] = 0;
				result[2][0] = 1;
				result[3][0] = 0;
				result[4][0] = - fu * xc * r2 / 2 * g3;



				//Derivatives of y_image:
				result[0][1] = 0;
				result[1][1] = mod_camframe[1];
				result[2][1] = 0;
				result[3][1] = 1;
				result[4][1] = - fv * yc * r2 / 2 * g3;

				return result;
			}

			/// Get the component of the motion of a point in the direction provided 
			///	with respect to each of the internal camera parameters
			/// @param direction The (x,y) direction to use
			inline TooN::Vector<num_parameters> get_parameter_derivs(const TooN::Vector<2>& direction) const
			{
				return get_parameter_derivs() * direction;
			}	

			/// Update the internal camera parameters by adding the vector given
			/// @param updates Update vector in the format 
			/// \f$ \begin{pmatrix}\Delta f_u & \Delta f_v & \Delta u_0 & \Delta v_0 & \Delta c\end{pmatrix} \f$
			//inline void update(const TooN::Vector<num_parameters>& updates);

			/// Returns the vector of camera parameters in the format
			/// \f$ \begin{pmatrix}f_u & f_v & u_0 & v_0 & c\end{pmatrix} \f$
			inline TooN::Vector<num_parameters>& get_parameters() 
			{
				return my_camera_parameters;
			}

		  private:
			TooN::Vector<num_parameters> my_camera_parameters; // f_u, f_v, u_0, v_0, alpha
			mutable TooN::Vector<2> my_last_camframe;
	};

	///This camera class is a very similar to the Harris model.
	///However, it forces the pixel aspect ratio to be unity.
	class SquareHarris{

		private:
			TooN::Vector<2> radial_distort(const TooN::Vector<2>& camframe) const
			{
				double r2 = camframe*camframe;
				return camframe / sqrt(1 + my_camera_parameters[4] * r2);
			}
			

		public:
			/// The number of parameters in the camera
			static const int num_parameters=5;
    
			///Load parameters from a stream 
			///@param is The stream to use
			inline void load(std::istream& is);

			/// Save parameters to a stream 
			///@param os The stream to use
			inline void save(std::ostream& os)
			{
				os << my_camera_parameters;
			}


			/// Fast linear projection for working out what's there
			inline TooN::Vector<2> linearproject(const TooN::Vector<2>& camframe, double scale=1) const
			{
				return TooN::Vector<2>(scale * (camframe* my_camera_parameters[0]) + my_camera_parameters.slice<2,2>());
			}

			/// Project from Euclidean camera frame to image plane
			inline TooN::Vector<2> project(const TooN::Vector<2>& camframe) const 
			{
				my_last_camframe = camframe;
				return linearproject(radial_distort(camframe));
			}

			/// Project from image plane to a Euclidean camera
			inline TooN::Vector<2> unproject(const TooN::Vector<2>& imframe)
			{
				//Undo the focal length and optic axis.
				TooN::Vector<2> mod_camframe;
				mod_camframe[0] = (imframe[0]-my_camera_parameters[2])/my_camera_parameters[0];
				mod_camframe[1] = (imframe[1]-my_camera_parameters[3])/my_camera_parameters[0];
				double rprime2 = mod_camframe*mod_camframe;
				my_last_camframe =  mod_camframe / sqrt(1 - my_camera_parameters[4] * rprime2);
				return my_last_camframe;
			}
			
			/// Get the derivative of image frame wrt camera frame at the last computed projection
			/// in the form \f$ \begin{bmatrix} \frac{\partial \text{im1}}{\partial \text{cam1}} & \frac{\partial \text{im1}}{\partial \text{cam2}} \\ \frac{\partial \text{im2}}{\partial \text{cam1}} & \frac{\partial \text{im2}}{\partial \text{cam2}} \end{bmatrix} \f$
			inline TooN::Matrix<2,2> get_derivative() const
			{
				TooN::Matrix<2,2> J;

				double xc = my_last_camframe[0];
				double yc = my_last_camframe[1];

				double f= my_camera_parameters[0];
				double a = my_camera_parameters[4];

				double g = 1/sqrt(1 + a * (xc*xc + yc*yc));
				double g3= g*g*g;
				
				J[0][0] = f * (g - 2 * a * xc*xc*g3);
				J[0][1] = -2 * f * a * xc * yc * g3;
				J[1][0] = -2 * f * a * xc * yc * g3;
				J[1][1] = f * (g - 2 * a * yc*yc*g3);

				return J;
			}

			/// Get the motion of a point with respect to each of the internal camera parameters
			inline TooN::Matrix<num_parameters,2> get_parameter_derivs() const 
			{
				TooN::Vector<2> mod_camframe = radial_distort(my_last_camframe);

				TooN::Matrix<5, 2> result;

				double xc = my_last_camframe[0];
				double yc = my_last_camframe[1];
				double r2 = xc*xc + yc*yc;

				double f= my_camera_parameters[0];
				double a = my_camera_parameters[4];

				double g = 1/sqrt(1 + a * r2);
				double g3= g*g*g;

				//Derivatives of x_image:
				result[0][0] = mod_camframe[0];
				result[1][0] = 0;
				result[2][0] = 1;
				result[3][0] = 0;
				result[4][0] = - f * xc * r2 / 2 * g3;



				//Derivatives of y_image:
				result[0][1] = mod_camframe[1];
				result[1][1] = 0;
				result[2][1] = 0;
				result[3][1] = 1;
				result[4][1] = - f * yc * r2 / 2 * g3;

				return result;
			}

			/// Get the component of the motion of a point in the direction provided 
			///	with respect to each of the internal camera parameters
			/// @param direction The (x,y) direction to use
			inline TooN::Vector<num_parameters> get_parameter_derivs(const TooN::Vector<2>& direction) const
			{
				return get_parameter_derivs() * direction;
			}	

			/// Update the internal camera parameters by adding the vector given
			/// @param updates Update vector in the format 
			/// \f$ \begin{pmatrix}\Delta f_u & \Delta f_v & \Delta u_0 & \Delta v_0 & \Delta c\end{pmatrix} \f$
			//inline void update(const TooN::Vector<num_parameters>& updates);

			/// Returns the vector of camera parameters in the format
			/// \f$ \begin{pmatrix}f_u & f_v & u_0 & v_0 & c\end{pmatrix} \f$
			inline TooN::Vector<num_parameters>& get_parameters() 
			{
				my_camera_parameters[1] = 0;
				return my_camera_parameters;
			}

		  private:
			TooN::Vector<num_parameters> my_camera_parameters; // f_u, f_v, u_0, v_0, alpha
			mutable TooN::Vector<2> my_last_camframe;
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

inline TooN::Vector<2> Camera::Linear::linearproject(const TooN::Vector<2>& camframe, double scale) const {
  return TooN::Vector<2>(scale * diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline TooN::Vector<2> Camera::Linear::project(const TooN::Vector<2>& camframe) const {
  my_last_camframe = camframe;
  return TooN::Vector<2>(diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline TooN::Vector<2> Camera::Linear::unproject(const TooN::Vector<2>& imframe) const {
  my_last_camframe[0] = (imframe[0]-my_camera_parameters[2])/my_camera_parameters[0];
  my_last_camframe[1] = (imframe[1]-my_camera_parameters[3])/my_camera_parameters[1];
  return my_last_camframe;
}

TooN::Matrix<2,2> Camera::Linear::get_derivative() const {
  TooN::Matrix<2,2> result;
  result(0,0) = my_camera_parameters[0];
  result(1,1) = my_camera_parameters[1];
  result(0,1) = result(1,0) = 0;
  return result;
}

TooN::Matrix<Camera::Linear::num_parameters,2> Camera::Linear::get_parameter_derivs() const {
  TooN::Matrix<num_parameters,2> result;
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

TooN::Vector<Camera::Linear::num_parameters> Camera::Linear::get_parameter_derivs(const TooN::Vector<2>& direction) const {
  TooN::Vector<num_parameters> result;
  result[0] = my_last_camframe[0] * direction[0];
  result[1] = my_last_camframe[1] * direction[1];
  result[2] = direction[0];
  result[3] = direction[1];

  return result;
}

void Camera::Linear::update(const TooN::Vector<num_parameters>& updates){
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

inline TooN::Vector<2> Camera::Cubic::linearproject(const TooN::Vector<2>& camframe, double scale){
  return TooN::Vector<2>(scale * diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline TooN::Vector<2> Camera::Cubic::project(const TooN::Vector<2>& camframe) const{
  my_last_camframe = camframe;
  TooN::Vector<2> mod_camframe = camframe * (1+SAT(my_camera_parameters[4]*(camframe*camframe)));
  return TooN::Vector<2>(diagmult(mod_camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline TooN::Vector<2> Camera::Cubic::unproject(const TooN::Vector<2>& imframe){
  TooN::Vector<2> mod_camframe;
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

TooN::Matrix<2,2> Camera::Cubic::get_derivative(){
  TooN::Matrix<2,2> result = TooN::Identity;
  result *= 1+my_camera_parameters[4]*(my_last_camframe*my_last_camframe);
  result += (2*my_camera_parameters[4]*my_last_camframe.as_col()) * my_last_camframe.as_row();
  result[0] *= my_camera_parameters[0];
  result[1] *= my_camera_parameters[1];
  return result;
}

TooN::Matrix<Camera::Cubic::num_parameters,2> Camera::Cubic::get_parameter_derivs() const {
  TooN::Vector<2> mod_camframe = my_last_camframe * (1+my_camera_parameters[4]*(my_last_camframe*my_last_camframe));
  TooN::Matrix<num_parameters,2> result;
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

TooN::Vector<Camera::Cubic::num_parameters> Camera::Cubic::get_parameter_derivs(const TooN::Vector<2>& direction) const {
  TooN::Vector<2> mod_camframe = my_last_camframe * (1+my_camera_parameters[4]*(my_last_camframe*my_last_camframe));
  TooN::Vector<num_parameters> result;
  result[0] = mod_camframe[0] * direction[0] *my_camera_parameters[0];
  result[1] = mod_camframe[1] * direction[1] *my_camera_parameters[1];
  result[2] = direction[0] *my_camera_parameters[0];
  result[3] = direction[1] *my_camera_parameters[1];
  result[4] = (diagmult(my_last_camframe,my_camera_parameters.slice<0,2>())*direction)*(my_last_camframe*my_last_camframe);
  return result;
}

void Camera::Cubic::update(const TooN::Vector<num_parameters>& updates){
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

inline TooN::Vector<2> Camera::Quintic::linearproject(const TooN::Vector<2>& camframe, double scale) const {
  return TooN::Vector<2>(scale * diagmult(camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline TooN::Vector<2> Camera::Quintic::project(const TooN::Vector<2>& camframe) const {
  my_last_camframe = camframe;
  double sc = /*sat*/(camframe*camframe);
  TooN::Vector<2> mod_camframe = camframe * (1 + sc*(my_camera_parameters[4] + sc*my_camera_parameters[5]));
  return TooN::Vector<2>(diagmult(mod_camframe, my_camera_parameters.slice<0,2>()) + my_camera_parameters.slice<2,2>());
}

inline std::pair<TooN::Vector<2>, TooN::Matrix<2> > Camera::Quintic::project(const TooN::Vector<2>& camframe, const TooN::Matrix<2>& R) const
{
    std::pair<TooN::Vector<2>, TooN::Matrix<2> > result;
    result.first = this->project(camframe);
    const TooN::Matrix<2> J = this->get_derivative();
    result.second = J * R * J.T();
    return result;
}

inline TooN::Vector<2> Camera::Quintic::unproject(const TooN::Vector<2>& imframe) const {
  TooN::Vector<2> mod_camframe;
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

inline std::pair<TooN::Vector<2>, TooN::Matrix<2> > Camera::Quintic::unproject(const TooN::Vector<2>& imframe, const TooN::Matrix<2>& R) const
{
    std::pair<TooN::Vector<2>, TooN::Matrix<2> > result;
    result.first = this->unproject(imframe);
    TooN::Matrix<2> J = get_derivative();
    double rdet = 1.0/ (J[0][0] * J[1][1] - J[0][1] * J[1][0]);
    TooN::Matrix<2> Jinv;
    Jinv[0][0] = rdet * J[1][1];
    Jinv[1][1] = rdet * J[0][0];
    Jinv[0][1] = -rdet * J[0][1];
    Jinv[1][0] = -rdet * J[1][0];    
    result.second = Jinv * R * Jinv.T();
    return result;
}


TooN::Matrix<2,2> Camera::Quintic::get_derivative() const {
  TooN::Matrix<2,2> result = TooN::Identity;
  double temp1=my_last_camframe*my_last_camframe;
  double temp2=my_camera_parameters[5]*temp1;
  result *= 1+temp1*(my_camera_parameters[4]+temp2);
  result += (2*(my_camera_parameters[4]+2*temp2)*my_last_camframe.as_col()) * my_last_camframe.as_row();
  result[0] *= my_camera_parameters[0];
  result[1] *= my_camera_parameters[1];
  return result;
}



TooN::Matrix<2,2> Camera::Quintic::get_inv_derivative() const {
  TooN::Matrix<2,2> result = TooN::Identity;
  double temp1=my_last_camframe*my_last_camframe;
  double temp2=my_camera_parameters[5]*temp1;
  double temp3=2.0*(my_camera_parameters[4]+2.0*temp2);
   
  result *= 1+temp1*(my_camera_parameters[4]+temp2);

  result[0][0] +=  my_last_camframe[1]*my_last_camframe[1]*temp3;
  result[0][1]  =-(temp3*my_last_camframe[0]*my_last_camframe[1]);
  
  result[1][1] +=  my_last_camframe[0]*my_last_camframe[0]*temp3;
  result[1][0]  =-(temp3*my_last_camframe[0]*my_last_camframe[1]);
  
  (result.T())[0] *= my_camera_parameters[1];
  (result.T())[1] *= my_camera_parameters[0];

  result /= (result[0][0]*result[1][1] - result[1][0]*result[0][1]);
  
  return result;
}

TooN::Matrix<2,2> Camera::Quintic::get_inv_derivative(const TooN::Vector<2>& x) const
{

  TooN::Matrix<2,2> result = TooN::Identity;
  double temp1=x*x;
  double temp2=my_camera_parameters[5]*temp1;
  double temp3=2.0*(my_camera_parameters[4]+2.0*temp2);

    result *= 1+temp1*(my_camera_parameters[4]+temp2);
  //Identity(result,1+temp1*(my_camera_parameters[4]+temp2));

  result[0][0] +=  x[1]*x[1]*temp3;
  result[0][1]  =-(temp3*x[0]*x[1]);
  
  result[1][1] +=  x[0]*x[0]*temp3;
  result[1][0]  =-(temp3*x[0]*x[1]);
  
  (result.T())[0] *= my_camera_parameters[1];
  (result.T())[1] *= my_camera_parameters[0];

  result /= (result[0][0]*result[1][1] - result[1][0]*result[0][1]);
  
  return result;

}

TooN::Matrix<2,2> Camera::Quintic::get_derivative(const TooN::Vector<2>& x) const {
    TooN::Matrix<2,2> result = TooN::Identity;
    double temp1=x*x;
    double temp2=my_camera_parameters[5]*temp1;
    result *= 1+temp1*(my_camera_parameters[4]+temp2);
    //Identity(result,1+temp1*(my_camera_parameters[4]+temp2));
    result += (2*(my_camera_parameters[4]+2*temp2)*x.as_col()) * x.as_row();
    result[0] *= my_camera_parameters[0];
    result[1] *= my_camera_parameters[1];
    return result;
}

TooN::Matrix<Camera::Quintic::num_parameters,2> Camera::Quintic::get_parameter_derivs() const {
  TooN::Matrix<num_parameters,2> result;
  double r2 = my_last_camframe * my_last_camframe;
  double r4 = r2 * r2;
  TooN::Vector<2> mod_camframe = my_last_camframe * (1+ r2 * (my_camera_parameters[4] + r2 * my_camera_parameters[5]));

  result(0,0) = mod_camframe[0];
  result(1,0) = 0;
  result(2,0) = 1;
  result(3,0) = 0;
  result(4,0) = my_camera_parameters[0]*my_last_camframe[0]*r2;
  result(5,0) = my_camera_parameters[0]*my_last_camframe[0]*r4;

  result(0,1) = 0;
  result(1,1) = mod_camframe[1];
  result(2,1) = 0;
  result(3,1) = 1;
  result(4,1) = my_camera_parameters[1]*my_last_camframe[1]*r2;
  result(5,1) = my_camera_parameters[1]*my_last_camframe[1]*r4;
  
  //cout<<"Finish me!\n";					     
  return result;
}

TooN::Vector<Camera::Quintic::num_parameters> Camera::Quintic::get_parameter_derivs(const TooN::Vector<2>& direction) const {
  //TooN::Vector<num_parameters> result;
  //cout<<"Finish me!\n";					     
  //FIXME improve this somewhat
  return get_parameter_derivs() * direction;
}

void Camera::Quintic::update(const TooN::Vector<num_parameters>& updates){
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
