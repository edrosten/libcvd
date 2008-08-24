#include <Python.h>
#include <numpy/arrayobject.h>
#include <vector>
#include <sstream>
#include <cvd/image.h>
#include <cvd/image_io.h>
#include <cvd/vector_image_ref.h>
#include <cvd/convolution.h>
#include <cvd/python/interface.h>

using namespace std;
using namespace CVD;

////////////////////////////////////////////////////////////////////////////////////
///
///                             convolveGaussian Wrapper
///
////////////////////////////////////////////////////////////////////////////////////

SELECTOR_START(convolveGaussianWrap)

  static PyObject* fun(PyArrayObject *py_image, double sigma)
  {

    SELECTOR_FUNC_START();

    if (PyArray_TYPE(py_image) == PyCVD::NumpyType<type>::num) {
      BasicImage <type> input(PyCVD::fromNumpyToBasicImage<type>(py_image, "image"));
      PyArrayObject *py_result;
      BasicImage <type> result(PyCVD::allocateNumpyCVDImageSiblings<type>(input.size().x,
									  input.size().y, &py_result));
      
      //convolveGaussian<type>(input, result, sigma);
      convolveGaussian(input, result, sigma);
      return (PyObject*)py_result;
    }
    else {
      return SELECTOR_NEXT(convolveGaussianWrap)(py_image, sigma);
    }
  }

SELECTOR_ERROR(convolveGaussianWrap)

  static PyObject* fun(PyArrayObject *image, double sigma)
  {
    throw string("Can't convolve image of unsupported type: " + PyArray_TYPE(image));
  }

SELECTOR_END

extern "C" PyObject *convolveGaussian(PyObject *self, PyObject *args) {
  try{
    PyArrayObject *_image;
    double sigma;

    cerr << "foo\n";
    if (!PyArg_ParseTuple(args, "O!d", &PyArray_Type, &_image, &sigma)) {
      return 0;
    }
    cerr << "bar\n";
    return SELECTOR_INVOKE(convolveGaussianWrap, PyCVD::CVDFloatTypes)(_image, sigma);
  }
  catch(string err) {
    cerr << "bing\n";
    PyErr_SetString(PyExc_RuntimeError, err.c_str());
    return 0;	
  }
}

//////////////////////////////////////////////// END END END convolveGaussian Wrapper

static PyMethodDef _cvd_wrap_methods[] = {
  {"convolveGaussian",
   convolveGaussian, METH_VARARGS},
  {NULL, NULL}     /* Sentinel - marks the end of this structure */
};

extern "C" void initcvd(void)  {
  (void) Py_InitModule("cvd", _cvd_wrap_methods);
  import_array();  // Must be present for NumPy.  Called first after above line.
}
