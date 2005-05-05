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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <iostream>
#include <cstdlib>
#include <cvd/se3.h>

using namespace std;
using namespace CVD;



int main(int argc, char** argv)
{
	
	if(argc != 7)
	{
		cerr << argv[0] << "error: put pre multiplication SE3 on command line";
		return 1;
	}


	Vector<6> camera;

	for(int i=0; i< 6; i++)
		camera[i] = atof(argv[i+1]);


	Vector<6> in;

	SE3 s3, c;

	c = SE3::exp(camera);

	for(;;)
	{
		cin >> in;

		if(!cin.good())
			return 0;

		s3=c * SE3::exp(in);

		cout << s3.ln() << endl;
	}
}
