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
// A WLS class using Cholesky decomposition and sparse JtJ
// Also stores the sum squared error and can compute the residual

#ifndef __WLS_C_H
#define __WLS_C_H

#ifdef __GNUC__
#warning This file is deprecated and evil. Please use WLSCHolesky defined in cvd/wls_cholesky.h
#else //elif defined(which ever compilers support this)
#pragma warning "This file is deprecated and evil. Please use WLSCHolesky defined in cvd/wls_cholesky.h"
#endif

#include <cvd/wls_cholesky.h>

#define WLS WLSCholesky

#endif
