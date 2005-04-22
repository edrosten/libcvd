#include <cvd/videodisplay.h>
#include <cvd/image_io.h>
#include <cvd/gl_helpers.h>
#include <X11/keysym.h>
#include <iostream.h>

using namespace CVD;
using namespace std;

int main(int argc, char** argv)
{
	if(argc != 1)
	{
		cerr << "Error: incorrect arguments\n.";
		cerr << "usage: " << argv[0] << " < image\n";
		return 1;
	}

	Image<Rgb<byte> > im;

	try
	{
		img_load(im, cin);
		
		VideoDisplay d(0, 0, im.size().x, im.size().y);

		d.select_events(KeyPressMask | ExposureMask);

		for(;;)
		{
			XEvent e;

			d.get_event(&e);

			if(e.type == KeyPress)
			{
				KeySym key;
				XLookupString(&e.xkey,	0, 0, &key, 0);

				if(key == XK_Escape || key == XK_q)
					break;
			}
			else if(e.type == Expose)
				glDrawPixels(im);
		}
	}
	catch(Exceptions::All b0rk)
	{
		cerr << "Error: " << b0rk.what << endl;
		return 1;
	}

	return 0;
}
