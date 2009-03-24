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

#include <cvd/draw.h>
#include <utility>
#include <TooN/helpers.h>
using namespace TooN;
using namespace std;

namespace CVD {

static double len2(const Vector<2>& v)
{
	return v*v;
}	


vector<ImageRef> getSolidEllipse(float r1, float r2, float theta)
{
	vector<ImageRef> e;

	int r = (int) ceil(max(r1, r2) + 1);
	Matrix<2> t;
	t[0] =  makeVector(cos(theta), sin(theta)) / r1;
	t[1] =  makeVector(-sin(theta), cos(theta)) / r2;


	for(int y=-r; y <= r; y++)
		for(int x=-r; x <= r; x++)
			if(len2(t * makeVector(x, y)) <= 1)
				e.push_back(ImageRef(x, y));
	return e;
}

};
