#include <iostream>
#include <fstream>
#include <cvd/rgb.h>

using namespace std;
using namespace CVD;

int main()
{
	cout << "#ifndef PNM_ARCH_H\n"
		 << "  #define PNM_ARCH_H\n";


	//Packed RGB types?

	if(sizeof(Rgb<unsigned char>) == 3)
		cout << "  #define PACKED_RGB\n";


	if(sizeof(short) > 1)
	{
		//Endian.
		union
		{
			unsigned char c[2];
			unsigned short s;
		} u;

		u.s=1;
		if(u.c[0])
			cout << "  #define ARCH_LITTLE_ENDIAN\n";

		//IO of shorts	
		short ss,s=0x3355;
		
		ofstream o;
		o.open("tmp");
		o.write((char*)&s, sizeof(s));
		o.close();

		ifstream i;
		i.open("tmp");
		i.read((char*)&ss, sizeof(ss));
		i.close();

		if(s==ss)
			cout << "  #define FAST_SHORT_IO\n";
	}

	cout << "#endif\n";
}
