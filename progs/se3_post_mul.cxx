#include <iostream>
#include <cstdlib>
#include <cvd/se3.h>

using namespace std;
using namespace CVD;



int main(int argc, char** argv)
{
	
	if(argc != 7)
	{
		cerr << argv[0] << "error: put post multiplication SE3 on command line";
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

		s3=SE3::exp(in) * c;

		cout << s3.ln() << endl;
	}
}
