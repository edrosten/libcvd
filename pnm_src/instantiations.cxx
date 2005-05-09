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
#include <cvd/image_convert.h>

#include <iostream>
using namespace std;

namespace CVD
{
namespace Pixel
{

	//This file contains instantiations of all the misc. things that need to be
	//instantiated. 

	//Some useful rgb weightings
	WeightedRGB<>		uniform(1./3, 1./3, 1./3);
	WeightedRGB<>		CIE(0.299, 0.587, 0.114);
	WeightedRGB<>		red_only(1, 0, 0);
	WeightedRGB<>		green_only(0, 1, 0);
	WeightedRGB<>		blue_only(0, 0, 1);


}
}
