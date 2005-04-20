//-*- c++ -*-
// A WLS class using Cholesky decomposition and sparse JtJ
// Also stores the sum squared error and can compute the residual

#ifndef __WLS_C_H
#define __WLS_C_H

#ifdef __GNUC__
#warning This file is deprecated and evil. Please use WLSCHolesky defined in cvd/wls_cholesky.h
#else //elif defined(which ever compilers support this)
#pragma warning "This file is deprecated and evil. Please use WLSCHolesky defined in cvd/wls_cholesky.h"
#endif

#include <cvb/wls_cholesky.h>

#define WLS WLSCholesky

#endif
