/**
 * types.h
 *
 * @author Damian Eads
 * @author Edward Rosten
 *
 * This file is part of the CVD Library.
 * Copyright (C) 2005 The Authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 * 02110-1301  USA
 */

#ifndef PYCVD_TYPES_HPP
#define PYCVD_TYPES_HPP

namespace PyCVD {


#define DEFINE_NUMPY_TYPE(Type, PyType)		\
template<> struct NumpyType<Type>\
{\
  static const int   num = PyType;			\
  static std::string name(){ return #Type;}		\
  static char code(){ return PyType##LTR;}		\
}

  template<class C> struct NumpyType
  {
  };

  DEFINE_NUMPY_TYPE(unsigned char , NPY_UBYTE );
  DEFINE_NUMPY_TYPE(char          , NPY_BYTE );
  DEFINE_NUMPY_TYPE(short         , NPY_SHORT );
  DEFINE_NUMPY_TYPE(unsigned short, NPY_USHORT);
  DEFINE_NUMPY_TYPE(int           , NPY_INT   );
  DEFINE_NUMPY_TYPE(long long     , NPY_LONGLONG);
  DEFINE_NUMPY_TYPE(unsigned int  , NPY_UINT  );
  DEFINE_NUMPY_TYPE(float         , NPY_FLOAT );
  DEFINE_NUMPY_TYPE(double        , NPY_DOUBLE);

  struct End{};

  template<class C, class D> struct TypeList
  {
    typedef C type;
    typedef D next;
  };


  typedef TypeList<char,
		   TypeList<unsigned char, 
		   TypeList<short,
                   TypeList<unsigned short,
		   TypeList<int,
		   TypeList<long long,
		   TypeList<unsigned int,
                   TypeList<float,
		   TypeList<double, End> > > > > > > > > CVDTypes;



  typedef TypeList<float,
                   TypeList<double, End> > CVDFloatTypes;
}

#endif
