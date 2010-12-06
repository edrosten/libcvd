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
#include <typeinfo>
#include <cstdlib>
#include <cvd/rgb.h>
#include <cvd/byte.h>
#include <cvd/glwindow.h>
#include <cvd/gl_helpers.h>
#include <cvd/videosource.h>

#ifdef CVD_HAVE_V4LBUFFER
	#include <cvd/Linux/v4lbuffer.h>
#endif

using namespace std;
using namespace CVD;

template<class C> void play(string s)
{
	VideoBuffer<C> *buffer = open_video_source<C>(s);
	
	GLWindow display(buffer->size());
	glDrawBuffer(GL_BACK);
	
	//while(buffer->frame_pending())
	
	cout << "FPS: " << buffer->frame_rate() << endl;
	cout << "Size: " << buffer->size() << endl;

	RawVideoBuffer* root = buffer->root_buffer();

	cout << typeid(*root).name() << endl;

	#ifdef CVD_HAVE_V4LBUFFER
		if(dynamic_cast<V4L::RawV4LBuffer*>(root))
		{
			cout << "Using V4LBuffer.\n";
		}
	#endif

	bool f=1;
	GLWindow::EventSummary e;
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	for(;;)
	{
		display.get_events(e);
		if(e.should_quit())
			break;

		VideoFrame<C>* frame = buffer->get_frame();

		glViewport(0, 0, display.size().x, display.size().y);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glRasterPos2f(-1, 1);
		glOrtho(-0.375, display.size().x-0.375, display.size().y-0.375, -0.375, -1 , 1); //offsets to make (0,0) the top left pixel (rather than off the display)

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameterf( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexImage2D(*frame, 0, GL_TEXTURE_RECTANGLE_NV);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex2i(0,0);
		glTexCoord2i(frame->size().x, 0);
		glVertex2i(display.size().x,0);
		glTexCoord2i(frame->size().x,frame->size().y);
		glVertex2i(display.size().x,display.size().y);
		glTexCoord2i(0, frame->size().y);
		glVertex2i(0, display.size().y);
		glEnd ();


		if(f)
		{
			cout << "frame size: " << frame->size() << endl;
			f=0;
		}
		buffer->put_frame(frame);
		glFlush();

		display.swap_buffers();
	}

	cout << "Exiting\n";
}

int main(int argc, char* argv[])
{
	int type =0;
	
	int arg=1;
	
	if(argc-1 >=1)
	{
	    if(argv[arg] == string("-mono"))
		{
			arg++;
			type=1;
		}
	    else if(argv[arg] == string("-rgb8"))
		{
			arg++;
			type=2;
		}
	}

	if(arg != argc-1)
	{	
		cerr << "Error: specify the video source\n";
		return 1;
	}
	try
	{
//		if(type == 1)
//			play<byte>(argv[arg]);
//		else if(type == 2)
//			play<Rgb8>(argv[arg]);
//		else
			play<Rgb<byte> >(argv[arg]);
	}
	catch(CVD::Exceptions::All& e)
	{
		cout << "Error: " << e.what << endl;
	}
}
