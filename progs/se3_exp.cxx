/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software, see LICENSE file for details
*/
#include <TooN/se3.h>
#include <iostream>

using namespace std;
using namespace TooN;

int main()
{
	SE3<> se3;
	Vector<6> v;

	cin >> v;
	se3 = SE3<>::exp(v);

	Matrix<3, 4> m;
	m.slice<0, 0, 3, 3>() = se3.get_rotation().get_matrix();
	m.T()[3] = se3.get_translation();

	cout << m;
	return 0;
}
