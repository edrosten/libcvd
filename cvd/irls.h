//-*- c++ -*-
#ifndef __IRLS_H
#define __IRLS_H

#include <cvd/wls.h>
#include <assert.h>
#include <cmath>

namespace CVD {



  // a couple of useful reweighting classes
struct RobustI {
  double sd_inlier;
  inline double reweight(double d) {return 1/(sd_inlier+fabs(d));}  // w(x)
  inline double true_scale(double d) {return reweight(d) - fabs(d)*reweight(d)*reweight(d);}  // w(x) + x w'(x)
  inline double objective(double d) {return fabs(d) + sd_inlier*log(sd_inlier*reweight(d));}  // integral (x w(x) )
};

struct RobustII {
  double sd_inlier;
  inline double reweight(double d){return 1/(sd_inlier+d*d);}
  inline double true_scale(double d){return d - 2*d*reweight(d);}
  inline double objective(double d){return 0.5 * log(1 + d*d/sd_inlier);}
};

struct ILinear {
  inline double reweight(double d){return 1;}
  inline double true_scale(double d){return 1;}
  inline double objective(double d){return d*d;}
};


template <int Size, class Reweight>
class IRLS
  : public Reweight,
    public WLS<Size>{
public:
  IRLS(){Identity(my_true_C_inv,0);my_residual=0;}

  inline void add_df(double d, const Vector<Size>& f) {
    double scale = reweight(d);
    double ts = true_scale(d);
    my_residual += objective(d);

    WLS<Size>::add_df(d,f,scale);

    for(int i=0; i<Size; i++){
      for(int j=0; j<Size; j++){
	my_true_C_inv[i][j]+=f[i]*f[j]*ts;
      }
    }
  }

  void operator += (const IRLS& meas){
    WLS<Size>::operator+=(meas);
    my_true_C_inv += meas.my_true_C_inv;
  }


  Matrix<Size,Size,RowMajor>& get_true_C_inv() {return my_true_C_inv;}
  const Matrix<Size,Size,RowMajor>& get_true_C_inv()const {return my_true_C_inv;}

  double get_residual() {return my_residual;}

private:

  double my_residual;

  Matrix<Size,Size,RowMajor> my_true_C_inv;

  // comment out to allow bitwise copying
  IRLS( IRLS& copyof );
  int operator = ( IRLS& copyof );
};

}

#endif
