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
#ifndef __SE3_H
#define __SE3_H

#include <cvd/so3.h>
#include <TooN/TooN.h>

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
  TooN::Vector<3>& get_translation() {return my_translation;}
  /// @overload
  const TooN::Vector<3>& get_translation() const {return my_translation;}

  /// Exponentiate a Vector in the Lie Algebra to generate a new SE3.
  /// See the Detailed Description for details of this vector.
  /// @param vect The Vector to exponentiate
    static SE3 exp(const TooN::Vector<6>& vect);
  /// Take the logarithm of the matrix, generating the corresponding vector in the Lie Algebra.
  /// See the Detailed Description for details of this vector.
  TooN::Vector<6> ln() const;

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

  /// Returns the i-th generator multiplied by a vector. 
  /// The generators of a Lie group are the basis for the space of the Lie algebra.
  /// For %SE3, the generators are six \f$4\times4\f$ matrices representing
  /// the six possible (linearised) degrees of freedom. These matrices are usally sparse,
  /// and are usually obtained to immediately multiply them by a vector, so
  /// this function provides a fast way of doing this operation.
  /// @param i The required generator
  /// @param pos The vector to multuiply by the generator
  static TooN::Vector<4> generator_field(int i, TooN::Vector<4> pos);

  /// Transfer a vector in the Lie Algebra from one
  /// co-ordinate frame to another. This is the operation such that for a matrix 
  /// \f$ B \f$, 
  /// \f$ e^{\text{Adj}(v)} = Be^{v}B^{-1} \f$
  /// @param v The Vector to transfer
  template<class Accessor>
  inline void adjoint(TooN::FixedVector<6,Accessor>& v)const;

  /// Transfer a matrix in the Lie Algebra from one
  /// co-ordinate frame to another. This is the operation such that for a matrix 
  /// \f$ B \f$, 
  /// \f$ e^{\text{Adj}(v)} = Be^{v}B^{-1} \f$
  /// @param M The Matrix to transfer
  template <class Accessor>
  inline void adjoint(TooN::FixedMatrix<6,6,Accessor>& M)const;

  /// Transfer covectors between frames (using the transpose of the inverse of the adjoint)
  /// so that trinvadjoint(vect1) * adjoint(vect2) = vect1 * vect2
  /// @param v The Vector to transfer
  template<class Accessor>
  inline void trinvadjoint(TooN::FixedVector<6,Accessor>& v)const;

  ///@overload
  template <class Accessor>
  inline void trinvadjoint(TooN::FixedMatrix<6,6,Accessor>& M)const;

private:
  SO3 my_rotation;
  TooN::Vector<3> my_translation;
};


// Member functions

// transfers a vector in the Lie algebra
// from one coord frame to another
// so that exp(adjoint(vect)) = (*this) * exp(vect) * (this->inverse())
template<class Accessor>
inline void SE3::adjoint(TooN::FixedVector<6,Accessor>& vect)const{
  vect.template slice<3,3>() = my_rotation * vect.template slice<3,3>();
  vect.template slice<0,3>() = my_rotation * vect.template slice<0,3>();
  vect.template slice<0,3>() += my_translation ^ vect.template slice<3,3>();
}

// Transfers covectors between frames
// (using the transpose of the inverse of the adjoint)
// so that trinvadjoint(vect1) * adjoint(vect2) = vect1 * vect2
template<class Accessor>
inline void SE3::trinvadjoint(TooN::FixedVector<6,Accessor>& vect)const{
  vect.template slice<3,3>() = my_rotation * vect.template slice<3,3>();
  vect.template slice<0,3>() = my_rotation * vect.template slice<0,3>();
  vect.template slice<3,3>() += my_translation ^ vect.template slice<0,3>();
}

template <class Accessor>
inline void SE3::adjoint(TooN::FixedMatrix<6,6,Accessor>& M)const{
  for(int i=0; i<6; i++){
    adjoint(M.T()[i]);
  }
  for(int i=0; i<6; i++){
    adjoint(M[i]);
  }
}
  
template <class Accessor>
inline void SE3::trinvadjoint(TooN::FixedMatrix<6,6,Accessor>& M)const{
  for(int i=0; i<6; i++){
    trinvadjoint(M.T()[i]);
  }
  for(int i=0; i<6; i++){
    trinvadjoint(M[i]);
  }
}

// Other related functions


/// Left-multiply an SE3 by an SO3 
/// @param lhs The rotation matrix
/// @param rhs The transformation matrix
/// @relates SE3
SE3 operator*(const SO3& lhs, const SE3& rhs);


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

#ifndef DOXYGEN_IGNORE_INTERNAL
///Helper function to multiply an SE3 by a Vector
/// @relates SE3
template<class VectorType>
struct SE3VMult {
  inline static void eval(TooN::Vector<4>& ret, const SE3& lhs, const VectorType& rhs){
    ret.template slice<0,3>()=lhs.get_rotation()*rhs.template slice<0,3>();
    ret.template slice<0,3>()+=lhs.get_translation() * rhs[3];
    ret[3] = rhs[3];
  }
};
#endif

/// Right-multiply by a Vector
/// @param lhs The transformation matrix
/// @param rhs The vector
/// @relates SE3
template<class Accessor>
TooN::Vector<4> operator*(const SE3& lhs, const TooN::FixedVector<4,Accessor>& rhs){
  return TooN::Vector<4>(lhs,rhs,TooN::Operator<SE3VMult<TooN::FixedVector<4, Accessor> > >());
}

///@overload
template<class Accessor>
TooN::Vector<4> operator*(const SE3& lhs, const TooN::DynamicVector<Accessor>& rhs){
	//FIXME: size checking
  return TooN::Vector<4>(lhs,rhs,TooN::Operator<SE3VMult<TooN::DynamicVector<Accessor> > >());
}




//////////////////
// operator *   //
// Vector * SE3 //
//////////////////

#ifndef DOXYGEN_IGNORE_INTERNAL
/// @internal
/// Helper function to multiply a vector by an SE3
/// @relates SE3
template<class Accessor>
struct VSE3Mult {
  inline static void eval(TooN::Vector<4>& ret, const TooN::FixedVector<4,Accessor>& lhs, const SE3& rhs){
    ret.template slice<0,3>() = lhs.template slice<0,3>() * rhs.get_rotation();
    ret[3] = lhs[3];
    ret[3] += lhs.template slice<0,3>() * rhs.get_translation();
  }
};
#endif

/// Left-multiply by a Vector
/// @param lhs The vector
/// @param rhs The rotation matrix
/// @relates SE3
template<class Accessor>
TooN::Vector<4> operator*(const TooN::FixedVector<4,Accessor>& lhs, const SE3& rhs){
  return TooN::Vector<4>(lhs,rhs,TooN::Operator<VSE3Mult<Accessor> >());
}





//////////////////
// operator *   //
// SE3 * Matrix //
//////////////////

#ifndef DOXYGEN_IGNORE_INTERNAL
/// Helper function to multipy an SE3 by a matrix plib
/// @relates SE3
template <int RHS, class Accessor>
struct SE3MMult {
  inline static void eval(TooN::Matrix<4,RHS>& ret, const SE3& lhs, const TooN::FixedMatrix<4,RHS,Accessor>& rhs){
    for(int i=0; i<RHS; i++){
      ret.T()[i].template slice<0,3>() = lhs.get_rotation() * rhs.T()[i].template slice<0,3>();
      ret.T()[i].template slice<0,3>() += lhs.get_translation() * rhs(3,i);
      ret(3,i) = rhs(3,i);
    }
  }
};
#endif


/// Right-multiply by a Matrix
/// @param lhs The transformation matrix
/// @param rhs The matrix
/// @relates SE3
template <int RHS, class Accessor>
TooN::Matrix<4,RHS> operator*(const SE3& lhs, const TooN::FixedMatrix<4,RHS,Accessor>& rhs){
  return TooN::Matrix<4,RHS>(lhs,rhs,TooN::Operator<SE3MMult<RHS,Accessor> >());
}


//////////////////
// operator *   //
// Matrix * SE3 //
//////////////////

#ifndef DOXYGEN_IGNORE_INTERNAL
/// Helper function to multipy a matrix by an SE3
/// @relates SE3
template <int LHS, class Accessor>
struct MSE3Mult {
  inline static void eval(TooN::Matrix<LHS,4>& ret, const TooN::FixedMatrix<LHS,4,Accessor>& lhs, const SE3& rhs){
    for(int i=0; i<LHS; i++){
      ret[i].template slice<0,3>() = lhs[i].template slice<0,3>() * rhs.get_rotation();
      ret(i,3) = rhs.get_translation() * lhs[i].template slice<0,3>();
      ret(i,3) += lhs(i,3);
    }
  }
};
#endif

/// Left-multiply by a Matrix
/// @param lhs The matrix
/// @param rhs The transformation matrix
/// @relates SE3
template <int LHS, class Accessor>
TooN::Matrix<LHS,4> operator*(const TooN::FixedMatrix<LHS,4,Accessor>& lhs, const SE3& rhs){
  return TooN::Matrix<LHS,4>(lhs,rhs,TooN::Operator<MSE3Mult<LHS,Accessor> >());
}


}

#endif
