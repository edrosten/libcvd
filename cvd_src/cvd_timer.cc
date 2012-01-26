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

cvd_timer::cvd_timer()
{
	startTimeInNanoSeconds = get_time_of_day_ns();
}

cvd_timer::cvd_timer(double t)
{
	reset(t);
}

double cvd_timer::reset() 
{
  long long temp  = get_time_of_day_ns();
  double elapsed =  (temp - startTimeInNanoSeconds) / 1e9;
  startTimeInNanoSeconds = temp;
  return elapsed;
}

double cvd_timer::get_time() 
{
  return (get_time_of_day_ns()-startTimeInNanoSeconds)/1e9;
}

// Conv from units of nanosecs (specifically for v4l2 : kernel 2.4
double cvd_timer::conv_ntime(signed long long time)
{
  return (time-startTimeInNanoSeconds)/1e9;
}

// Conv from units of nanosecs (specifically for v4l2 : kernel 2.6
// Let the overloading mecnanism sort out the difference in the 
// headers from 2.4 hee, hee, hee
double cvd_timer::conv_ntime(const struct timeval& tv)
{
	double time = tv.tv_sec + tv.tv_usec*1e-6;
	return time-startTimeInNanoSeconds / 1e9;
}

double get_time_of_day() 
{
  return get_time_of_day_ns()/1e9;
}


void cvd_timer::reset(const double t)
{
	startTimeInNanoSeconds = (long long)(t * 1e9);
}

void cvd_timer::reset_ns(long long t)
{
	startTimeInNanoSeconds = t;
}

cvd_timer timer;

}
