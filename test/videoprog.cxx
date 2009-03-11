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
#include <cvd/videodisplay.h>
#include <cvd/image_io.h>
#include <cvd/gl_helpers.h>


#include <fstream>
#include <stdio.h>
#include <list>
#include <X11/keysym.h>
#include <X11/Xlib.h>

using namespace CVD;
using namespace std;

int mymain(int /*argc*/, char** argv)
{
	VideoBuffer<pix>* vbuf = get_vbuf();
	VideoDisplay vd(0, 0, vbuf->size().x, vbuf->size().y);
	
	XEvent e;
	vd.select_events(ButtonPress|KeyPressMask);

	int  saved_name=0;
	int video_sequence = 0, capturing_video = 0, frame_num=0;

	list<Image<pix> > video;

	VideoFrame<pix>*  vf;
	for(;;)
	{
		vf = vbuf->get_frame();

		while(vd.pending())
		{
			vd.get_event(&e);
			if(e.type == ButtonPress)
			{
				delete vbuf;
				return 0;
			} 
			else if(e.type == KeyPress)
			{
				KeySym k;

				XLookupString(&e.xkey, 0, 0, &k, 0);

				if(k == XK_c)
				{
					char buf[100];
					char *name;
					
					name=strrchr(argv[0], '/');

					if(!name) 
						name=argv[0];
					else
						name++;
					
					sprintf(buf, "capture-%s-%05i.pnm", name, saved_name);

					ofstream out;

					out.open(buf);
					pnm_save(*vf, out);
					out.close();

					std::cout << "Saved: " << buf << "\n";

					saved_name++;
				}
				else if(k == XK_Escape || k == XK_q || k == XK_Q)
				{
					delete vbuf;
					return 0;
		
				}
				else if(k == XK_r)
				{
					if(!capturing_video)
						capturing_video=1, frame_num=0;
					else
					{
						capturing_video = 0;

						char buf[100];
						int n=0;

						for(list<Image<pix> >::iterator i=video.begin(); i != video.end(); i++)
						{
							sprintf(buf, "video_sequence-%03i-%05i.pnm", video_sequence, n++);
							ofstream out;
							out.open(buf);
							pnm_save(*i, out);
							out.close();
							cout << "Written: " << buf << endl;
						}

						video_sequence++;
					}

					video.clear();
				}
			}

		}

		if(capturing_video)
		{
			Image<pix> i;
			i.copy_from(*vf);
			video.push_back(i);
			cout << frame_num++ << " " <<flush;
		}
		glDrawPixels(*vf);
		glFlush();
		vbuf->put_frame(vf);
	}
	
	return 0;
}

int main(int argc, char** argv)
{
	try
	{
		mymain(argc, argv);
	}
	catch(Exceptions::All x)
	{
		cerr << "Error: " << x.what << endl;
		throw;
	}


	return 0;	
}
