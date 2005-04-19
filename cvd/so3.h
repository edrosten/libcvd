//-*- c++ -*-
#ifndef __SO3_H
#define __SO3_H

#include <numerics.h>

namespace CVD {

/// @ingroup gLinAlg 

/// Class to represent a three-dimensional rotation matrix. Three-dimensional rotation
/// matrices are members of the Special Orthogonal Lie group SO3. These can be parameterised
/// three numbers (in the space of the Lie Algebra). In this class, the three parameters are the
/// finite rotation vector i.e. a three-dimensional vector whose direction is the axis of rotation
/// and whose length is the angle of rotation in radians. Exponentiating this vector gives the matrix,
/// and the logarithm of the matrix gives this vector.
class SO3 {
public:
  friend inline std::istream& operator>>(std::istream& is, SO3& rhs);
  friend inline std::istream& operator>>(std::istream& is, class SE3& rhs);
  /// Default constructor. Initialises the matrix to the identity (no rotation)
  SO3();

  /// Assignment operator
  /// @param rhs The SO3 to copy
  SO3& operator=(const SO3& rhs);
  /// Assigment operator from a general matrix. If the matrix is not 
  /// already a valid rotation matrix, coerce().
  /// @param rhs The Matrix to copy
  SO3& operator=(const Matrix<3>& rhs);
  /// Modifies the matrix to make sure it is a valid rotation matrix.
  void coerce();

  /// Exponentiate a vector in the Lie algebra to generate a new SO3.
  /// See the Detailed Description for details of this vector.
  /// @param vect The Vector to exponentiate
  template<class Accessor>
  inline static SO3 exp(const FixedVector<3,Accessor>& vect) {return exp(vect.get_data_ptr());}
  /// Exponentiate a vector in the Lie algebra to generate a new SO3.
  /// See the Detailed Description for details of this vector.
  /// @param vect The three doubles to exponentiate
  static SO3 exp(const double* vect);


  /// Take the logarithm of the matrix, generating the corresponding vector in the Lie Algebra.
  /// See the Detailed Description for details of this vector.
  Vector<3> ln() const;

  /// Member access. Access is row major i.e. <code>[4]</code> will give the 2,1 element.
  double operator[](int i){return my_matrix[i/3][i%3];}

  /// Returns the inverse of this matrix (=the transpose, so this is a fast operation)
  SO3 inverse() const;

  /// Right-multiply by another rotation matrix
  /// @param rhs The SO3 to multiply by
  SO3& operator *=(const SO3& rhs){
    my_matrix=my_matrix*rhs.my_matrix;
    return *this;
  }

  /// Right-multiply by another rotation matrix
  /// @param rhs The SO3 to multiply by
  SO3 operator *(const SO3& rhs) const{
    return SO3(*this)*=rhs;
  }

  /// Returns the SO3 as a Matrix<3>
  const Matrix<3>& get_matrix()const {return my_matrix;}

  /// Returns the i-th generator multiplied by a vector (since that is what you usually want to do)
  static Vector<3> generator_field(int i, Vector<3> pos);

  /// Transfer a vector in the Lie Algebra from one
  /// co-ordinate frame to another such that for a matrix 
  /// \f$ M \f$, 
  /// \f$ e^{\text{Adj}(v)} = Me^{v}M^{-1} \f$
  /// @param v The Vector to transfer
  Vector<3> adjoint(Vector<3> v) const ;

 private:
  Matrix<3> my_matrix;
};

/// Write an SO3 to a stream 
/// @param os The stream
/// @param rhs The SO3
/// @relates SO3
inline std::ostream& operator<< (std::ostream& os, const SO3& rhs){
  return os << rhs.get_matrix();
}

/// Read from SO3 to a stream 
/// @param is The stream
/// @param rhs The SO3
/// @relates SO3
inline std::istream& operator>>(std::istream& is, SO3& rhs){
  return is >> rhs.my_matrix;
}



/// Right-multiply by a Vector
/// @param lhs The rotation matrix
/// @param rhs The vector
/// @relates SO3
template<class Accessor>
Vector<3> operator*(const SO3& lhs, const DynamicVector<Accessor>& rhs){
  //FIXME: check size
  return lhs.get_matrix() * rhs;
}

///@overload
/// @relates SO3
template<class Accessor>
Vector<3> operator*(const SO3& lhs, const FixedVector<3,Accessor>& rhs){
  return lhs.get_matrix() * rhs;
}



/// Left-multiply by a Vector
/// @param lhs The vector
/// @param rhs The rotation matrix
/// @relates SO3
template<class Accessor>
Vector<3> operator*(const DynamicVector<Accessor>& lhs, const SO3& rhs){
  //FIXME: check size
  return lhs * rhs.get_matrix();
}

///@overload
/// @relates SO3
template<class Accessor>
Vector<3> operator*(const FixedVector<3,Accessor>& lhs, const SO3& rhs){
  return lhs * rhs.get_matrix();
}

/// Multiply two SO3 matrices
/// @param lhs The left-hand rotation matrix
/// @param rhs The right-hand rotation matrix
/// @relates SO3
template <int LHS, class Accessor>
Matrix<LHS,3> operator*(const FixedMatrix<LHS,3,Accessor>& lhs, const SO3& rhs){
  return lhs * rhs.get_matrix();
}

///@overload
/// @relates SO3
template <int RHS, class Accessor>
Matrix<3,RHS> operator*(const SO3& lhs, const FixedMatrix<3,RHS,Accessor>& rhs){
  return lhs.get_matrix() * rhs;
}


}

#endif
