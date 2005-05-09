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
//Expexts video buffer valled vbuf and frame?* vf and display vd
	
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


			}

