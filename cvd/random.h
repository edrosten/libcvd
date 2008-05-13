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
#ifndef CVD_RANDOM_H
#define CVD_RANDOM_H

#include <cstdlib>
#include <cmath>

namespace CVD {
	/// Uniform random numbers between 0 and 1
	///
	/// This uses rand() internally, so set thw seed with srand()
	/// @ingroup gMaths
	inline double rand_u()
	{
		return ((double) std::rand()/ RAND_MAX);
	}

	/// Gaussian random numbers with zero mean and unit standard deviation.
	///
	/// This uses rand() internally, so set thw seed with srand()
	/// @ingroup gMaths
	inline double rand_g()
	{
		static bool use_old=false;
		static double y2;
		double r;


		if(!use_old)
		{
			double x1, x2, w, y1;
			do {
				x1 = 2.0 * rand_u() - 1.0;
				x2 = 2.0 * rand_u() - 1.0;
				w = x1 * x1 + x2 * x2;
			} while ( w >= 1.0 );

			w = std::sqrt( (-2.0 * std::log( w ) ) / w );
			y1 = x1 * w;
			y2 = x2 * w;

			r = y1;
			use_old = true;
		}
		else
		{
			r = y2;
			use_old = false;
		}


		return r;
	}

}

#endif
