/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software, see LICENSE file for details
*/
#include <TooN/se3.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace TooN;

int main(int argc, char** argv)
{

	if(argc != 7)
	{
		cerr << argv[0] << "error: put pre multiplication SE3 on command line";
		return 1;
	}

	Vector<6> camera;

	for(int i = 0; i < 6; i++)
		camera[i] = atof(argv[i + 1]);

	Vector<6> in;

	SE3<> s3, c;

	c = SE3<>::exp(camera);

	for(;;)
	{
		cin >> in;

		if(!cin.good())
			return 0;

		s3 = c * SE3<>::exp(in);

		cout << s3.ln() << endl;
	}
}
