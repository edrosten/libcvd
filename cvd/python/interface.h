/**
 * interface.h
 *
 * @author Damian Eads
 * @author Edward Rosten
 *
 * This file is part of the CVD Library.
 * Copyright (C) 2005 The Authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef PYCVD_INTERFACE_HPP
#define PYCVD_INTERFACE_HPP

#include <Python.h>
#include <numpy/arrayobject.h>
#include <vector>
#include <sstream>
#include <cvd/image.h>
#include <cvd/image_io.h>
#include <cvd/vector_image_ref.h>
#include <cvd/convolution.h>

#include "types.h"

////////////////////////////////////////////////////////////////////////////////
//
// This code provides the lists needed for python to C++ type mangling
//
////////////////////////////////////////////////////////////////////////////////

namespace PyCVD {

  template <class T>
  PyArrayObject *fromBasicImageToNumpy(const CVD::BasicImage<T> &image) {
    npy_intp a[] = {image.size().y, image.size().x};
    PyArrayObject *retval = (PyArrayObject*)PyArray_SimpleNew(2, a, NumpyType<T>::type);
    T *data = (T*)retval->data;
    copy(image.begin(), image.end(), data);
    return retval;
  }

  template <class T>
  void allocateNumpyCVDImageSiblings(int width, int height,
				     /**out: */ CVD::BasicImage<T> *image,
				     PyArrayObject **numpy) {
    npy_intp a[] = {height, width};
    *numpy = (PyArrayObject*)PyArray_SimpleNew(2, a, NumpyType<T>::type);
    *image = CVD::BasicImage<T>((T*)(*numpy)->data, CVD::ImageRef(width, height));
  }

}

#endif
