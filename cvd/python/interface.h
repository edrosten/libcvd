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
#include <sstream>
#include <string>
#include <cvd/image.h>
#include <cvd/image_io.h>
#include <cvd/convolution.h>

#include "types.h"
#include "selector.h"

////////////////////////////////////////////////////////////////////////////////
//
// This code provides the lists needed for python to C++ type mangling
//
////////////////////////////////////////////////////////////////////////////////

namespace PyCVD {

  template <class T>
  PyArrayObject *fromBasicImageToNumpy(const CVD::BasicImage<T> &image) {
    npy_intp a[] = {image.size().y, image.size().x};
    PyArrayObject *retval = (PyArrayObject*)PyArray_SimpleNew(2, a, NumpyType<T>::num);
    T *data = (T*)retval->data;
    copy(image.begin(), image.end(), data);
    return retval;
  }

  template <class T>
  CVD::BasicImage<T> allocateNumpyCVDImageSiblings(int width, int height,
						   /**out: */
						   PyArrayObject **numpy) {
    npy_intp a[] = {height, width};
    *numpy = (PyArrayObject*)PyArray_SimpleNew(2, a, NumpyType<T>::num);
    return CVD::BasicImage<T>((T*)(*numpy)->data, CVD::ImageRef(width, height));
  }

  template<class I>
    CVD::BasicImage<I> fromNumpyToBasicImage(PyObject *p, const std::string &n="") {
    
    if (!PyArray_Check(p)
	|| PyArray_NDIM(p) != 2 
	|| !PyArray_ISCONTIGUOUS(p) 
	|| PyArray_TYPE(p) != NumpyType<I>::num) {
      throw std::string(n + " must be a contiguous array of " + NumpyType<I>::name() + " (type code " + NumpyType<I>::code() + ")!");
    }

    PyArrayObject* image = (PyArrayObject*)p;
    
    int sm = image->dimensions[1];
    int sn = image->dimensions[0];
    CVD::BasicImage <I> img((I*)image->data, CVD::ImageRef(sm, sn));
    return img;
  }

  template<class I>
    CVD::BasicImage<I> fromNumpyToBasicImage(PyArrayObject *image, const std::string& n="") {
    
    if (PyArray_NDIM(image) != 2 
	|| !PyArray_ISCONTIGUOUS(image) 
	|| PyArray_TYPE(image) != NumpyType<I>::num) {
      throw std::string(n + " must be a contiguous array of " + NumpyType<I>::name() + " (type code " + NumpyType<I>::code() + ")!");
    }
    
    int sm = image->dimensions[1];
    int sn = image->dimensions[0];
    CVD::BasicImage <I> img((I*)image->data, CVD::ImageRef(sm, sn));
    return img;
  }


}


#endif
