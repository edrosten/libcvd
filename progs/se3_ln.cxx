/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software, see LICENSE file for details
*/
#include <TooN/se3.h>
#include <iostream>

using namespace TooN;
using namespace std;

int main()
{
	SE3<> se3;

	cin >> se3;
	cout << se3.ln() << endl;
	return 0;
}
