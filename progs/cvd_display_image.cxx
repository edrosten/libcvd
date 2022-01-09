/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software, see LICENSE file for details
*/

#include <cvd/gl_helpers.h>
#include <cvd/image_io.h>
#include <cvd/videodisplay.h>

#include <iostream>

using namespace CVD;

int main()
{
	try
	{
		Image<Rgb<byte>> i = img_load(std::cin);

		VideoDisplay d(i.size());

		d.select_events(ExposureMask | KeyPressMask);

		for(;;)
		{
			glDrawPixels(i);
			glFlush();

			XEvent e;
			d.get_event(&e);

			if(e.type == KeyPress)
				return 0;
		}
	}
	catch(const Exceptions::All& a)
	{
		std::cerr << "Error: " << a.what() << std::endl;
	}
}
