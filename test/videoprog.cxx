#include <cvd/videodisplay.h>
#include <cvd/helpers.h>

#include <fstream>
#include <stdio.h>
#include <list>
#include <X11/keysym.h>
#include <X11/Xlib.h>

using namespace CVD;
using namespace std;

int mymain(int argc, char** argv)
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

		#ifdef HACK
			HACK
		#else
			vf = vbuf->get_frame();
		#endif

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
	
		glDrawPixels(vf->size().x, vf->size().y, DATA_TYPE, GL_UNSIGNED_BYTE, vf->data());

		#ifdef HACK
		#else
			vbuf->put_frame(vf);
		#endif
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
