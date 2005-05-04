//-*- c++ -*-
#ifndef __WLS_H
#define __WLS_H

#include <numerics.h>
#include <SVD.h>
#include <numhelpers.h>

#include <assert.h>
#include <cmath>

namespace CVD {

/// Performs weighted least squares computation.
/// @param Size The number of dimensions in the system
/// @ingroup gMaths
template <int Size>
class WLS {
public:
	/// Default constructor
  WLS(){clear();}
  /// Construct using a given regularisation prior
  WLS(double prior){clear(prior);}

	/// Resets the measurements and covariance to zero
  void clear(){
    Identity(my_C_inv,0);
    for(int i=0; i<Size; i++){
      my_vector[i]=0;
    }
  }

	/// Applies a constant regularisation term. 
	/// Equates to a prior that says all the parameters are zero with \f$\sigma^2 = \frac{1}{\text{val}}\f$.
	/// @param val The strength of the prior
  void add_prior(double val){
    for(int i=0; i<Size; i++){
      my_C_inv(i,i)+=val;
    }
  }
  
	/// Applies a regularisation term with a different strength for each parameter value. 
	/// Equates to a prior that says all the parameters are zero with \f$\sigma_i^2 = \frac{1}{\text{v}_i}\f$.
	/// @param v The vector of priors
  template<class Accessor>
  void add_prior(const FixedVector<Size,Accessor>& v){
    for(int i=0; i<Size; i++){
      my_C_inv(i,i)+=v[i];
    }
  }

	/// Applies a whole-matrix regularisation term. 
	/// This is the same as adding the \f$m\f$ to the inverse covariance matrix.
	/// @param m The inverse covariance matrix to add
  template<class Accessor>
  void add_prior(const FixedMatrix<Size,Size,Accessor>& m){
    my_C_inv+=m;
  }

	/// Add a single measurement 
	/// @param m The value of the measurement
	/// @param J The Jacobian for the measurement \f$\frac{\partial\text{m}}{\partial\text{param}_i}\f$
	/// @param weight The inverse variance of the measurement (default = 1)
  template<class Accessor>
  inline void add_df(double m, const FixedVector<Size,Accessor>& J, double weight = 1) {
    Vector<Size> Jw = J*weight;
    for(int i=0; i<Size; i++){
      for(int j=0; j<Size; j++){
	my_C_inv[i][j]+=J[i]*Jw[j];
      }
      my_vector[i]+=Jw[i]*m;
    }
  }

	/// Add multiple measurements at once (much more efficiently)
	/// @param N The number of measurements
	/// @param m The measurements to add
	/// @param J The Jacobian matrix \f$\frac{\partial\text{m}_i}{\partial\text{param}_j}\f$
	/// @param invcov The inverse covariance of the measurement values
  template<int N, class Accessor1, class Accessor2, class Accessor3>
  inline void add_df(const FixedVector<N,Accessor1>& m,
		     const FixedMatrix<Size,N,Accessor2>& J,
		     const FixedMatrix<N,N,Accessor3>& invcov){
    my_C_inv += J * invcov * J.T();
    my_vector += J * invcov * m;
  }

  void compute(){
    my_svd.compute(my_C_inv);
    my_mu=my_svd.backsub(my_vector);
  }

  /// Combine measurements from two WLS systems
  /// @param meas The measurements to combine with
  void operator += (const WLS& meas){
    my_vector+=meas.my_vector;
    my_C_inv += meas.my_C_inv;
    my_true_C_inv += meas.my_true_C_inv;
  }

  /// Returns the inverse covariance matrix
  Matrix<Size,Size,RowMajor>& get_C_inv() {return my_C_inv;}
  /// Returns the inverse covariance matrix
  const Matrix<Size,Size,RowMajor>& get_C_inv() const {return my_C_inv;}
  Vector<Size>& get_mu(){return my_mu;}
  const Vector<Size>& get_mu() const {return my_mu;}
  Vector<Size>& get_vector(){return my_vector;}
  const Vector<Size>& get_vector() const {return my_vector;}
  SVD<Size>& get_svd(){return my_svd;}
  const SVD<Size>& get_svd() const {return my_svd;}

private:
  Vector<Size> my_mu;
  Matrix<Size,Size,RowMajor> my_C_inv;
  Vector<Size> my_vector;
  SVD<Size,Size> my_svd;

  // comment out to allow bitwise copying
  WLS( WLS& copyof );
  int operator = ( WLS& copyof );
};

}

#endif
