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

#ifndef CVD_INC_INTERPOLATE_H
#define CVD_INC_INTERPOLATE_H

#include <cvd/image.h>
#include <cvd/config.h>
#include <cvd/vision.h>

namespace CVD
{
	///Interploate a 1D local extremem by fitting a quadratic tho the three data points and interpolating.
	///The middle argument must be the most extreme, and the extremum
	///position is returned relative to 0.
	///
	///Arguments are checked for extremeness by means of assert.
	///
	///@param d1  Data point value for $x=-1$
	///@param d2  Data point value for $x=0$
	///@param d3  Data point value for $x=1$
	///@return The $x$ coordinate of the extremum.
	///@ingroup gVision
	double interpolate_extremum(double d1, double d2, double d3)
	{	
		assert(d2 >= d1 && d2 > d3 || d2 > d1 && d2 >= d3);
		assert(d2 <= d1 && d2 < d3 || d2 < d1 && d2 <= d3);
		//Use Quadratic interpolation to find the peak, position
		//and hence the "real" edge position.
		return -0.5 * (d3 - d1) / (d3 + d1 - 2 * d2);
	}

	#if defined CVD_HAVE_TOON || defined DOXYGEN_IGNORE_INTERNAL

	///Interpolate a 2D local maximum, by fitting a quadratic. This is done by using
	///using the 9 datapoints to compute the local Hessian using finite differences and
	///finding the location where the gradient  is zero.
	///
	/// Given the grid of pixels:
	/// <pre>
	/// a b c
	/// d e f
	/// g h i
	/// </pre>
	/// The centre pixel (e) must be the most extreme of all the pixels.
	///
	///@param I__1__1 Pixel $(-1, -1)$ relative to the centre (a) 
	///@param I__1_0  Pixel $(-1,  0)$ relative to the centre (b) 
	///@param I__1_1  Pixel $(-1,  1)$ relative to the centre (c) 
	///@param I_0__1  Pixel $( 0, -1)$ relative to the centre (d) 
	///@param I_0_0   Pixel $( 0,  0)$ relative to the centre (e) 
	///@param I_0_1   Pixel $( 0,  1)$ relative to the centre (f) 
	///@param I_1__1  Pixel $( 1, -1)$ relative to the centre (g) 
	///@param I_1_0   Pixel $( 1,  0)$ relative to the centre (h) 
	///@param I_1_1   Pixel $( 1,  1)$ relative to the centre (i) 
	///@param Location of the local extrema.
	///@ingroup gVision
	TooN::Vector<2> interpolate_extremum(double I__1__1,
		                                 double I__1_0,
		                                 double I__1_1,
		                                 double I_0__1,
		                                 double I_0_0,
		                                 double I_0_1,
		                                 double I_1__1,
		                                 double I_1_0,
		                                 double I_1_1)
	{
		//Compute the gradient values
		double gx = 0.5 * (I_1_0 - I__1_0);
		double gy = 0.5 * (I_0_1 - I_0__1);

		//Compute the Hessian values
		double gxx = I_1_0 - 2 * I_0_0 + I__1_0;
		double gyy = I_0_1 - 2 * I_0_0 + I_0__1;
		double gxy = 0.25 * (I_1_1 + I__1__1 - I_1__1 - I__1_1);

		//Compute -inv(H) * grad

		double Dinv = 1./(gxx * gyy - gxy * gxy);

		Vector<2> v;
		v[0] = -Dinv * (gyy * gx - gxy * gy);
		v[1] = -Dinv * (-gxy * gx + gxx * gy);
		
		return v;
	}
										 

	///Interpolate a 2D local maximum, by fitting a quadratic.
	///@param i Image in which to interpolate extremum
	///@param p Point at which to interpolate extremum
	///@return Pocation of local extremum
	///@ingroup gVision
	template<class I> TooN::Vector<2> interpolate_extremum(const SubImage<I>& i, ImageRef p)
	{
	    CVD_IMAGE_ASSERT(p.x > 0 && p.y > 0 && p.x < i.size().x-1 && p.y < i.size().y-1);
		
		//Extract and label 9 particular points
		float I__1__1 = i[p[j] + ImageRef(-1, -1)];
		float I__1_0  = i[p[j] + ImageRef(-1,  0)];
		float I__1_1  = i[p[j] + ImageRef(-1,  1)];
		float I_0__1  = i[p[j] + ImageRef( 0, -1)];
		float I_0_0   = i[p[j] + ImageRef( 0,  0)];
		float I_0_1   = i[p[j] + ImageRef( 0,  1)];
		float I_1__1  = i[p[j] + ImageRef( 1, -1)];
		float I_1_0   = i[p[j] + ImageRef( 1,  0)];
		float I_1_1   = i[p[j] + ImageRef( 1,  1)];
		return interpolate_extremum(I__1__1, I__1_0, I__1_1, I_0__1, I_0_0, I_0_1, I_1__1, I_1_0, I_1_1);
	}
	
	#endif



}


#endif

