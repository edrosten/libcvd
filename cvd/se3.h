//-*- c++ -*-
#ifndef __SE3_H
#define __SE3_H

#include <cvd/so3.h>
#include <numerics.h>

namespace CVD {

/// Class to represent a three-dimensional Euclidean transformation (a rotation and a translation). 
/// This can be represented by a member of the Special Euclidean Lie group SE3. These can be parameterised
/// six numbers (in the space of the Lie Algebra). In this class, the first three parameters are a
/// translation vector while the second three are a rotation vector, whose direction is the axis of rotation
/// and length the amount of rotation (in radians), as for SO3
/// @ingroup gLinAlg
class SE3 {
  friend SE3 operator*(const SO3& lhs, const SE3& rhs);
  friend std::istream& operator>> (std::istream& is, SE3& rhs);
public:
	/// Default constructor. Initialises the the rotation to zero (the identity) and the translation to zero
  SE3();

  /// Returns the rotation part of the transformation as a SO3
  SO3& get_rotation(){return my_rotation;}
  /// @overload
  const SO3& get_rotation() const {return my_rotation;}
  /// Returns the translation part of the transformation as a Vector
  Vector<3>& get_translation() {return my_translation;}
  /// @overload
  const Vector<3>& get_translation() const {return my_translation;}

  /// Exponentiate a Vector in the Lie Algebra to generate a new SE3.
  /// See the Detailed Description for details of this vector.
  /// @param vect The Vector to exponentiate
    static SE3 exp(const Vector<6>& vect);
  /// Take the logarithm of the matrix, generating the corresponding vector in the Lie Algebra.
  /// See the Detailed Description for details of this vector.
  Vector<6> ln() const;

  /// Returns a SE3 representing the inverse transformation.
  /// An SE3 is \f$ \left[R|t \right] \f$ (where \f$R\f$ is a rotation matrix and
  /// \f$t\f$ the translation), so the inverse is \f$\left[R^T|-R^Tt\right]\f$
  SE3 inverse() const;

  /// Right-multiply by another SE3 (concatenate the two transformations)
  /// @param rhs The multipier
  SE3& operator *=(const SE3& rhs);
  /// Right-multiply by another SE3 (concatenate the two transformations)
  /// @param rhs The multipier
  SE3 operator *(const SE3& rhs) const;

  /// Returns the i-th generator multiplied by a vector (since that is what you usually want to do)
  static Vector<4> generator_field(int i, Vector<4> pos);

  /// Transfer a vector in the Lie Algebra from one
  /// co-ordinate frame to another such that for a matrix 
  /// \f$ M \f$, 
  /// \f$ e^{\text{Adj}(v)} = Me^{v}M^{-1} \f$
  /// @param v The Vector to transfer
  template<class Accessor>
  inline void adjoint(FixedVector<6,Accessor>& v)const;

  ///@overload
  template <class Accessor>
  inline void adjoint(FixedMatrix<6,6,Accessor>& M)const;

  /// Transfer covectors between frames (using the transpose of the inverse of the adjoint)
  /// so that trinvadjoint(vect1) * adjoint(vect2) = vect1 * vect2
  /// @param v The Vector to transfer
  template<class Accessor>
  inline void trinvadjoint(FixedVector<6,Accessor>& v)const;

  ///@overload
  template <class Accessor>
  inline void trinvadjoint(FixedMatrix<6,6,Accessor>& M)const;

private:
  SO3 my_rotation;
  Vector<3> my_translation;
};


/// Left-multiply an SE3 by an SO3 
/// @param lhs The rotation matrix
/// @param rhs The transformation matrix
/// @relates SE3
SE3 operator*(const SO3& lhs, const SE3& rhs);


// transfers a vector in the Lie algebra
// from one coord frame to another
// so that exp(adjoint(vect)) = (*this) * exp(vect) * (this->inverse())
template<class Accessor>
inline void SE3::adjoint(FixedVector<6,Accessor>& vect)const{
  vect.template slice<3,3>() = my_rotation * vect.template slice<3,3>();
  vect.template slice<0,3>() = my_rotation * vect.template slice<0,3>();
  vect.template slice<0,3>() += my_translation ^ vect.template slice<3,3>();
}

// tansfers covectors between frames
// (using the transpose of the inverse of the adjoint)
// so that trinvadjoint(vect1) * adjoint(vect2) = vect1 * vect2
template<class Accessor>
inline void SE3::trinvadjoint(FixedVector<6,Accessor>& vect)const{
  vect.template slice<3,3>() = my_rotation * vect.template slice<3,3>();
  vect.template slice<0,3>() = my_rotation * vect.template slice<0,3>();
  vect.template slice<3,3>() += my_translation ^ vect.template slice<0,3>();
}

template <class Accessor>
inline void SE3::adjoint(FixedMatrix<6,6,Accessor>& M)const{
  for(int i=0; i<6; i++){
    adjoint(M.T()[i]);
  }
  for(int i=0; i<6; i++){
    adjoint(M[i]);
  }
}
  
template <class Accessor>
inline void SE3::trinvadjoint(FixedMatrix<6,6,Accessor>& M)const{
  for(int i=0; i<6; i++){
    trinvadjoint(M.T()[i]);
  }
  for(int i=0; i<6; i++){
    trinvadjoint(M[i]);
  }
}


/// Write an SE3 to a stream 
/// @param os The stream
/// @param rhs The SE3
/// @relates SE3
inline std::ostream& operator <<(std::ostream& os, const SE3& rhs){
  for(int i=0; i<3; i++){
    os << rhs.get_rotation().get_matrix()[i] << rhs.get_translation()[i] << std::endl;
  }
  return os;
}

/// Reads an SE3 from a stream 
/// @param is The stream
/// @param rhs The SE3
/// @relates SE3
/// @relatesalso SO3
inline std::istream& operator>>(std::istream& is, SE3& rhs){
  for(int i=0; i<3; i++){
    is >> rhs.get_rotation().my_matrix[i] >> rhs.get_translation()[i];
  }
  return is;
}


//////////////////
// operator *   //
// SE3 * Vector //
//////////////////

///@internal
///Helper function to multiply an SE3 by a Vector
/// @relates SE3
template<class VectorType>
struct SE3VMult {
  inline static void eval(Vector<4>& ret, const SE3& lhs, const VectorType& rhs){
    ret.template slice<0,3>()=lhs.get_rotation()*rhs.template slice<0,3>();
    ret.template slice<0,3>()+=lhs.get_translation() * rhs[3];
    ret[3] = rhs[3];
  }
};

/// Right-multiply by a Vector
/// @param lhs The transformation matrix
/// @param rhs The vector
/// @relates SE3
template<class Accessor>
Vector<4> operator*(const SE3& lhs, const FixedVector<4,Accessor>& rhs){
  return Vector<4>(lhs,rhs,Operator<SE3VMult<FixedVector<4, Accessor> > >());
}

///@overload
template<class Accessor>
Vector<4> operator*(const SE3& lhs, const DynamicVector<Accessor>& rhs){
	//FIXME: size checking
  return Vector<4>(lhs,rhs,Operator<SE3VMult<DynamicVector<Accessor> > >());
}




//////////////////
// operator *   //
// Vector * SE3 //
//////////////////

/// @internal
/// Helper function to multiply a vector by an SE3
/// @relates SE3
template<class Accessor>
struct VSE3Mult {
  inline static void eval(Vector<4>& ret, const FixedVector<4,Accessor>& lhs, const SE3& rhs){
    ret.template slice<0,3>() = lhs.template slice<0,3>() * rhs.get_rotation();
    ret[3] = lhs[3];
    ret[3] += lhs.template slice<0,3>() * rhs.get_translation();
  }
};

/// Left-multiply by a Vector
/// @param lhs The vector
/// @param rhs The rotation matrix
/// @relates SE3
template<class Accessor>
Vector<4> operator*(const FixedVector<4,Accessor>& lhs, const SE3& rhs){
  return Vector<4>(lhs,rhs,Operator<VSE3Mult<Accessor> >());
}





//////////////////
// operator *   //
// SE3 * Matrix //
//////////////////

/// @internal
/// Helper function to multipy an SE3 by a matrix
/// @relates SE3
template <int RHS, class Accessor>
struct SE3MMult {
  inline static void eval(Matrix<4,RHS>& ret, const SE3& lhs, const FixedMatrix<4,RHS,Accessor>& rhs){
    for(int i=0; i<RHS; i++){
      ret.T()[i].template slice<0,3>() = lhs.get_rotation() * rhs.T()[i].template slice<0,3>();
      ret.T()[i].template slice<0,3>() += lhs.get_translation() * rhs(3,i);
      ret(3,i) = rhs(3,i);
    }
  }
};


/// Right-multiply by a Matrix
/// @param lhs The transformation matrix
/// @param rhs The matrix
/// @relates SE3
template <int RHS, class Accessor>
Matrix<4,RHS> operator*(const SE3& lhs, const FixedMatrix<4,RHS,Accessor>& rhs){
  return Matrix<4,RHS>(lhs,rhs,Operator<SE3MMult<RHS,Accessor> >());
}


//////////////////
// operator *   //
// Matrix * SE3 //
//////////////////

/// @internal
/// Helper function to multipy a matrix by an SE3
/// @relates SE3
template <int LHS, class Accessor>
struct MSE3Mult {
  inline static void eval(Matrix<LHS,4>& ret, const FixedMatrix<LHS,4,Accessor>& lhs, const SE3& rhs){
    for(int i=0; i<LHS; i++){
      ret[i].template slice<0,3>() = lhs[i].template slice<0,3>() * rhs.get_rotation();
      ret(i,3) = rhs.get_translation() * lhs[i].template slice<0,3>();
      ret(i,3) += lhs(i,3);
    }
  }
};


/// Left-multiply by a Matrix
/// @param lhs The matrix
/// @param rhs The transformation matrix
/// @relates SE3
template <int LHS, class Accessor>
Matrix<LHS,4> operator*(const FixedMatrix<LHS,4,Accessor>& lhs, const SE3& rhs){
  return Matrix<LHS,4>(lhs,rhs,Operator<MSE3Mult<LHS,Accessor> >());
}


}

#endif
