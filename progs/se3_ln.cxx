#include <iostream>
#include <cvd/se3.h>

using namespace CVD;
using namespace std;

int main()
{
	SE3 se3;

	cin >> se3;
	cout << se3.ln() << endl;
	return 0;
}
