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

#include <cvd/videodisplay.h>
#include <cvd/diskbuffer2.h>
#include <cvd/deinterlacebuffer.h>
#include <cvd/gl_helpers.h>

using namespace CVD;
using namespace std;

#define TYPE Rgb<byte>

#ifndef TYPE
#error Define a type
#endif


int main(int argc, char** argv)
{
	vector<string> av;

	for(int i=1; i < argc; i++)
		av.push_back(argv[i]);
	
	
	try
	{
		DiskBuffer2<TYPE > diskb(av, 25);
		DeinterlaceBuffer<TYPE> db(diskb, DeinterlaceBuffer<TYPE>::EvenOdd);

		VideoDisplay d(0, 0, db.size().x, db.size().y);
		

		while(1)
		{
			VideoFrame<TYPE > *vf = db.get_frame();
			glDrawPixels(*vf);
			db.put_frame(vf);
		}
	}
	catch(CVD::Exceptions::All e)
	{
		cout << "Error: " << e.what << endl;
	}
}

