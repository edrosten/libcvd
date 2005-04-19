//-*- c++ -*-
// A WLS class using Cholesky decomposition and sparse JtJ
// Also stores the sum squared error and can compute the residual

#ifndef __WLS_C_H
#define __WLS_C_H

#include <numerics.h>
#include <SVD.h>
#include <numhelpers.h>
#include <assert.h>
#include <cmath>

namespace CVD {

/// Performs weighted least squares using Cholesky decomposition and sparse JtJ.
/// Also stores the sum squares error and can compute the residual.
/// @ingroup gMaths
template <int Size>
class WLS {
public:
  WLS(){clear();}
  WLS(double prior){clear(prior);}
  WLS(const WLS &w) {
    my_C_inv=w.my_C_inv;
    my_err=w.my_err;
    my_extra=w.my_extra;
    my_vector=w.my_vector;
  }

  void clear(){
    Identity(my_C_inv,0);
    for(int i=0; i<Size; i++){
      my_vector[i]=0;
    }
    my_err=0;
    my_extra=0;
  }

  void clear(double prior){
    Identity(my_C_inv,prior);
    for(int i=0; i<Size; i++){
      my_vector[i]=0;
    }
    my_err=0;
    my_extra=0;
  }

  void add_prior(double val){
    for(int i=0; i<Size; i++){
      my_C_inv(i,i)+=val;
    }
  }
  
  template<class Accessor>
  void add_prior(const FixedVector<Size,Accessor>& v){
    for(int i=0; i<Size; i++){
      my_C_inv(i,i)+=v[i];
    }
  }

  template<class Accessor>
  void add_prior(const FixedMatrix<Size,Size,Accessor>& m){
    my_C_inv+=m;
  }

  template<class Accessor>
  inline void add_df(double d, const FixedVector<Size,Accessor>& f, double weight = 1) {
    Vector<Size> fw = f*weight;
    for(int i=0; i<Size; i++){
      for(int j=i; j<Size; j++){
	my_C_inv[i][j]+=f[i]*fw[j];
      }
      my_vector[i]+=fw[i]*d;
    }
    my_err+=d*weight*d;
  }

  // Brand new add_df function for adding multiple measurements at once
  // with non-trivial covariance
  template<int N, class Accessor1, class Accessor2, class Accessor3>
  inline void add_df(const FixedVector<N,Accessor1>& d,
		     const FixedMatrix<Size,N,Accessor2>& J,
		     const FixedMatrix<N,N,Accessor3>& invcov){
    my_C_inv += J * invcov * J.T();  // FIXME make me faster!
    Vector<N> temp(invcov*d);
    my_vector += J * temp;
    my_err += d*temp;
  }

  
  void compute(){
    // Homegrown Cholesky
    Matrix<Size> L;
    for(int i=0;i<Size;i++) {
      double a=my_C_inv[i][i];
      for(int k=0;k<i;k++) a-=L[k][i]*L[k][i];
      L[i][i]=sqrt(a);
      for(int j=i;j<Size;j++) {
	a=my_C_inv[i][j];
	for(int k=0;k<i;k++) a-=L[k][j]*L[k][i];
	L[i][j]=a/L[i][i];
      }
    }
    Vector<Size> y;
    for(int i=0;i<Size;i++) {
      double a=my_vector[i];
      for(int j=0;j<i;j++) a-=L[j][i]*y[j];
      y[i]=a/L[i][i];
    }
    for(int i=Size-1;i>-1;i--) {
      double a=y[i];
      for(int j=i+1;j<Size;j++) a-=L[i][j]*my_mu[j];
      my_mu[i]=a/L[i][i];
    } 
  }

  void operator += (const WLS& meas){
    my_vector+=meas.my_vector;
    my_C_inv+=meas.my_C_inv;
    my_err+=meas.my_err;
    my_extra+=meas.my_extra;
  }

  void operator = (const WLS &w) {
    my_C_inv=w.my_C_inv;
    my_err=w.my_err;
    my_vector=w.my_vector;
    my_extra=w.my_extra;
  }

  Matrix<Size,Size,RowMajor>& get_C_inv() {return my_C_inv;}
  const Matrix<Size,Size,RowMajor>& get_C_inv() const {return my_C_inv;}
  Vector<Size>& get_mu(){return my_mu;}
  const Vector<Size>& get_mu() const {return my_mu;}
  Vector<Size>& get_vector(){return my_vector;}
  const Vector<Size>& get_vector() const {return my_vector;}
  double get_residual(){
    Vector<Size> temp;
    Zero(temp);
    for(int ii=0;ii<Size;ii++) {
      temp[ii]+=my_C_inv[ii][ii]*my_mu[ii];
      for(int jj=ii+1;jj<Size;jj++) {
	temp[ii]+=my_C_inv[ii][jj]*my_mu[jj];
	temp[jj]+=my_C_inv[ii][jj]*my_mu[ii];
      }
    }
    return my_err-my_mu*temp;
  }

  inline void add_extra(double e) {my_extra+=e;}
  inline double get_extra() {return my_extra;}

private:
  Vector<Size> my_mu;
  Matrix<Size,Size,RowMajor> my_C_inv;
  Vector<Size> my_vector;
  double my_err;    // error before optimisation
  double my_extra;  // extra residual error 
};

}

#endif
