#include <iostream>
#include <cvd/se3.h>

using namespace CVD;
using namespace std;

int main()
{
	SE3 se3;
	Vector<6> v;

	cin >> v;
	se3 = SE3::exp(v);

	cout << (se3.inverse()).ln() << endl;
	return 0;
}
