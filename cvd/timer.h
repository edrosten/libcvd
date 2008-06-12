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
////////////////////////////////////////////////////////
// 
// A timer class designed for dealing with timestamps
// CK Nov 2002
//
////////////////////////////////////////////////////////

#ifndef __CVD_TIMER_H
#define __CVD_TIMER_H

struct timeval;

namespace CVD {

/// Provides the time elapsed in seconds. This predominantly a wrapper for
/// the system call 
/// gettimeofday(), but can also provides a convenient way of converting from other
/// time units. In all cases, the time is given relative to the time the class
/// was created, or the last time reset() was called.
/// @ingroup gCPP
class cvd_timer 
{
	public:
		/// Create a timer, and set the start time to be now
		cvd_timer();
		/// How many seconds have elapsed since the start time?
		double get_time();

		/// Convert the current time from units of nanoseconds into a double,
		/// correcting for the start time
		/// @param time The current time in nanoseconds
		double conv_ntime(signed long long time);  

		/// Convert the current time from a timeval into a double, correcting
		/// for the start time
		/// @param tv The current time as a timeval
		double conv_ntime(const struct timeval& tv);

		/// Convert current time given as double by correcting for the start time
		/// @param time current time as double
		double conv_ntime(const double time) const {
			return time - startTime * 1.e-6;
		}

		/// Sets the start time to the current time
		double reset();                       


	private:
		unsigned long long startTime;
};
 
/// A instance of a cvd_timer for general use
/// @ingroup gCPP
extern cvd_timer timer;

/// Same as the system call gettimeofday, but returns time since
/// the epoch as a double.
double get_time_of_day();

 
}

#endif
