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
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  videoplay.C                                                            //
//                                                                         //
//  Test program for videofilebuffer                                       //
//                                                                         //
//  Paul Smith    30 April 2004                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include <cvd/image.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>
#include <cvd/videodisplay.h>
#include <cvd/gl_helpers.h>
#include <cvd/videosource.h>

using namespace std;
using namespace CVD;

template<class C> void play(string s)
{
	VideoBuffer<C> *buffer = open_video_source<C>(s);
	
	VideoDisplay display(buffer->size());
	
	while(buffer->frame_pending())
	{
		VideoFrame<C>* frame = buffer->get_frame();
		glDrawPixels(*frame);
		buffer->put_frame(frame);
				  glFlush();
	}
}

int main(int argc, char* argv[])
{
	bool mono=0;
	
	int arg=1;
	
	if(argc-1 >=1 && argv[arg] == string("-mono"))
	{
		arg++;
		mono=1;
	}

	if(arg != argc-1)
	{	
		cerr << "Error: specify the video source\n";
		return 1;
	}
	try
	{
		if(mono)
			play<byte>(argv[arg]);
		else
			play<Rgb<byte> >(argv[arg]);
	}
	catch(CVD::Exceptions::All& e)
	{
		cout << "Error: " << e.what << endl;
	}
}
