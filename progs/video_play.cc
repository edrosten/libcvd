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
#include <cvd/videofilebuffer.h>

using namespace std;
using namespace CVD;

int main(int argc, char* argv[])
{
	try
	{
		VideoFileBuffer<Rgb<byte> > buffer(argv[1]);	
		buffer.on_end_of_buffer(VideoBufferFlags::UnsetPending);
		
		VideoDisplay display(0, 0, buffer.size().x, buffer.size().y);
		
		while(buffer.frame_pending())
		{
			VideoFrame<Rgb<byte> >* frame = buffer.get_frame();
			glDrawPixels(*frame);
			buffer.put_frame(frame);
                      glFlush();
		}
	}
	catch(CVD::Exceptions::All& e)
	{
		cout << "Error: " << e.what << endl;
	}
}
