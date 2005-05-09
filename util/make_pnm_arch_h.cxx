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
