//-*- c++ -*-
#ifndef __WLS_H
#define __WLS_H

#include <numerics.h>
#include <SVD.h>
#include <numhelpers.h>

#include <assert.h>
#include <cmath>

namespace CVD {

template <int Size>
class WLS {
public:
  WLS(){clear();}

  void clear(){
    Identity(my_C_inv,0);
    for(int i=0; i<Size; i++){
      my_vector[i]=0;
    }
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
      for(int j=0; j<Size; j++){
	my_C_inv[i][j]+=f[i]*fw[j];
      }
      my_vector[i]+=fw[i]*d;
    }
  }

  // Brand new add_df function for adding multiple measurements at once
  // with non-trivial covariance
  template<int N, class Accessor1, class Accessor2, class Accessor3>
  inline void add_df(const FixedVector<N,Accessor1>& d,
		     const FixedMatrix<Size,N,Accessor2>& J,
		     const FixedMatrix<N,N,Accessor3>& invcov){
    my_C_inv += J * invcov * J.T();
    my_vector += J * invcov * d;
  }

  
  void compute(){
    my_svd.compute(my_C_inv);
    my_mu=my_svd.backsub(my_vector);
  }

  void operator += (const WLS& meas){
    my_vector+=meas.my_vector;
    my_C_inv += meas.my_C_inv;
    my_true_C_inv += meas.my_true_C_inv;
  }

  Matrix<Size,Size,RowMajor>& get_C_inv() {return my_C_inv;}
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
