//-*- c++ -*-
#ifndef __SE3_H
#define __SE3_H

#include <cvd/so3.h>
#include <numerics.h>

namespace CVD {

class SE3 {
  friend SE3 operator*(const SO3& lhs, const SE3& rhs);
  friend std::istream& operator>> (std::istream& is, SE3& rhs);
public:
  SE3();

  SO3& get_rotation(){return my_rotation;}
  const SO3& get_rotation() const {return my_rotation;}
  Vector<3>& get_translation() {return my_translation;}
  const Vector<3>& get_translation() const {return my_translation;}

  static SE3 exp(const Vector<6>& vect);
  Vector<6> ln() const;

  SE3 inverse() const;

  SE3& operator *=(const SE3& rhs);
  SE3 operator *(const SE3& rhs) const;

  static Vector<4> generator_field(int i, Vector<4> pos);


  template<class Accessor>
  inline void adjoint(FixedVector<6,Accessor>& vect)const;

  template<class Accessor>
  inline void trinvadjoint(FixedVector<6,Accessor>& vect)const;

  template <class Accessor>
  inline void adjoint(FixedMatrix<6,6,Accessor>& M)const;

  template <class Accessor>
  inline void trinvadjoint(FixedMatrix<6,6,Accessor>& M)const;

private:
  SO3 my_rotation;
  Vector<3> my_translation;
};


// left multiply an SE3 by an SO3 
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


// operator ostream& <<
inline std::ostream& operator <<(std::ostream& os, const SE3& rhs){
  for(int i=0; i<3; i++){
    os << rhs.get_rotation().get_matrix()[i] << rhs.get_translation()[i] << std::endl;
  }
  return os;
}

// operator istream& >>
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

template<class VectorType>
struct SE3VMult {
  inline static void eval(Vector<4>& ret, const SE3& lhs, const VectorType& rhs){
    ret.template slice<0,3>()=lhs.get_rotation()*rhs.template slice<0,3>();
    ret.template slice<0,3>()+=lhs.get_translation() * rhs[3];
    ret[3] = rhs[3];
  }
};

template<class Accessor>
Vector<4> operator*(const SE3& lhs, const FixedVector<4,Accessor>& rhs){
  return Vector<4>(lhs,rhs,Operator<SE3VMult<FixedVector<4, Accessor> > >());
}

template<class Accessor>
Vector<4> operator*(const SE3& lhs, const DynamicVector<Accessor>& rhs){
	//FIXME: size checking
  return Vector<4>(lhs,rhs,Operator<SE3VMult<DynamicVector<Accessor> > >());
}




//////////////////
// operator *   //
// Vector * SE3 //
//////////////////

template<class Accessor>
struct VSE3Mult {
  inline static void eval(Vector<4>& ret, const FixedVector<4,Accessor>& lhs, const SE3& rhs){
    ret.template slice<0,3>() = lhs.template slice<0,3>() * rhs.get_rotation();
    ret[3] = lhs[3];
    ret[3] += lhs.template slice<0,3>() * rhs.get_translation();
  }
};

template<class Accessor>
Vector<4> operator*(const FixedVector<4,Accessor>& lhs, const SE3& rhs){
  return Vector<4>(lhs,rhs,Operator<VSE3Mult<Accessor> >());
}





//////////////////
// operator *   //
// SE3 * Matrix //
//////////////////

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


template <int RHS, class Accessor>
Matrix<4,RHS> operator*(const SE3& lhs, const FixedMatrix<4,RHS,Accessor>& rhs){
  return Matrix<4,RHS>(lhs,rhs,Operator<SE3MMult<RHS,Accessor> >());
}


//////////////////
// operator *   //
// Matrix * SE3 //
//////////////////

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


template <int LHS, class Accessor>
Matrix<LHS,4> operator*(const FixedMatrix<LHS,4,Accessor>& lhs, const SE3& rhs){
  return Matrix<LHS,4>(lhs,rhs,Operator<MSE3Mult<LHS,Accessor> >());
}







}

#endif
