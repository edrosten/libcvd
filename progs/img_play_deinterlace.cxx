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

