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

namespace CVD {
std::vector<ImageRef> getCircle(int radius) {
    std::vector<ImageRef> points;
    int y = 0;
    for (int x=-radius; x<=0; x++) {
        int nexty2 = radius*radius - ((x+1)*(x+1));
        while (true) {
        	points.push_back(ImageRef(x,y));
        	if (y*y >= nexty2)
        	  break;
        	++y;
        }
    }
    size_t i;
    for (i=points.size()-1;i>0;i--)
        points.push_back(ImageRef(-points[i-1].x, points[i-1].y));
    for (i=points.size()-1;i>1;i--)
        points.push_back(ImageRef(points[i-1].x, -points[i-1].y));
    return points;
}

std::vector<ImageRef> getDisc(float radius)
{
    std::vector<ImageRef> points;

	int r = (int)ceil(radius + 1);

	for(ImageRef p(0,-r); p.y <= r; p.y++)
		for(p.x = -r; p.x <= r; p.x++)
			if(p.mag_squared() <= radius*radius)
				points.push_back(p);


	
	return points;
}

};
