//-*- c++ -*-
#ifndef __SO3_H
#define __SO3_H

#include <numerics.h>

namespace CVD {

class SO3 {
public:
  friend inline std::istream& operator>>(std::istream& is, SO3& rhs);
  friend inline std::istream& operator>>(std::istream& is, class SE3& rhs);
  SO3();

  SO3& operator=(const SO3& rhs);
  SO3& operator=(const Matrix<3>& rhs);
  void coerce();

  static SO3 exp(const double* vect);

  template<class Accessor>
  inline static SO3 exp(const FixedVector<3,Accessor>& vect) {return exp(vect.get_data_ptr());}

  Vector<3> ln() const;

  double operator[](int i){return my_matrix[i/3][i%3];}

  SO3 inverse() const;

  SO3& operator *=(const SO3& rhs){
    my_matrix=my_matrix*rhs.my_matrix;
    return *this;
  }

  SO3 operator *(const SO3& rhs) const{
    return SO3(*this)*=rhs;
  }

  const Matrix<3>& get_matrix()const {return my_matrix;}

  // returns i-th generator times pos
  static Vector<3> generator_field(int i, Vector<3> pos);

  // adjoint transformation on the Lie algebra
  Vector<3> adjoint(Vector<3> vect) const ;

 private:
  Matrix<3> my_matrix;
};

inline std::ostream& operator<< (std::ostream& os, const SO3& rhs){
  return os << rhs.get_matrix();
}

inline std::istream& operator>>(std::istream& is, SO3& rhs){
  return is >> rhs.my_matrix;
}




template<class Accessor>
Vector<3> operator*(const SO3& lhs, const FixedVector<3,Accessor>& rhs){
  return lhs.get_matrix() * rhs;
}

template<class Accessor>
Vector<3> operator*(const SO3& lhs, const DynamicVector<Accessor>& rhs){
  //FIXME: check size
  return lhs.get_matrix() * rhs;
}




template<class Accessor>
Vector<3> operator*(const DynamicVector<Accessor>& lhs, const SO3& rhs){
  //FIXME: check size
  return lhs * rhs.get_matrix();
}

template<class Accessor>
Vector<3> operator*(const FixedVector<3,Accessor>& lhs, const SO3& rhs){
  return lhs * rhs.get_matrix();
}

template <int RHS, class Accessor>
Matrix<3,RHS> operator*(const SO3& lhs, const FixedMatrix<3,RHS,Accessor>& rhs){
  return lhs.get_matrix() * rhs;
}

template <int LHS, class Accessor>
Matrix<LHS,3> operator*(const FixedMatrix<LHS,3,Accessor>& lhs, const SO3& rhs){
  return lhs * rhs.get_matrix();
}


}

#endif
