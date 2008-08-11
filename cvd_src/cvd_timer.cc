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
///////////////////////////////////////////////////////
// 
// A timer class designed for dealing with timestamps
// CK Nov 2002
//
///////////////////////////////////////////////////////

#include "cvd/timer.h"
#include <iostream>

#if defined(WIN32) && !defined(__MINGW32__)
#include "Win32/win32.h"
#else
#include <sys/time.h>  //gettimeofday
#endif

namespace CVD {

unsigned long long get_time_of_day_us()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (unsigned long long)tv.tv_sec*1000000+tv.tv_usec;
}

cvd_timer::cvd_timer()
{
	startTime = get_time_of_day_us();
}

double cvd_timer::reset() 
{
  unsigned long long temp  = get_time_of_day_us();
  double elapsed =  (temp - startTime) / 1000000.0;
  startTime = temp;
  return elapsed;
}

double cvd_timer::get_time() 
{
  return (get_time_of_day_us()-startTime)/1000000.0;
}

// Conv from units of nanosecs (specifically for v4l2 : kernel 2.4
double cvd_timer::conv_ntime(signed long long time)
{
  time=time/1000;  // now in us

  return (time-startTime)/1000000.0;
}

// Conv from units of nanosecs (specifically for v4l2 : kernel 2.6
// Let the overloading mecnanism sort out the difference in the 
// headers from 2.4 hee, hee, hee
double cvd_timer::conv_ntime(const struct timeval& tv)
{
	double time = tv.tv_sec + tv.tv_usec*1e-6;
	return time-startTime / 1e6;
}

double get_time_of_day() 
{
  return get_time_of_day_us()/1000000.0;
}


cvd_timer timer;

}
