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
		VideoFileBuffer buffer(argv[1]);	
		buffer.on_end_of_buffer(VideoBufferFlags::UnsetPending);
		
		VideoDisplay display(0, 0, buffer.size().x, buffer.size().y);
		
		while(buffer.frame_pending())
		{
			VideoFrame<Rgb<byte> >* frame = buffer.get_frame();
			glDrawPixels(*frame);
			buffer.put_frame(frame);
		}
	}
	catch(CVD::Exceptions::All& e)
	{
		cout << "Error: " << e.what << endl;
	}
}
