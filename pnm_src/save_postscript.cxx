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
#include <cvd/internal/io/save_postscript.h>

#include <cvd/internal/load_and_save.h>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

namespace CVD
{

void output_eps_header(ostream& o, int xs, int ys)
{
	//Standard EPS header. This is test more by concensus from various postscript handlers
	//than with any specific reference to documentation, but it seems to work.
	o << 
	"%!PS-Adobe-3.0 EPSF-3.0"																"\n"
	"%%BoundingBox: 0 0 " << xs << " " << ys << 											"\n"
	"%%Creator: libCVD"																		"\n"
	"%%Pages: 0"																			"\n"	
	"%%EndComments"																			"\n"	
	"%%EndProlog"																			"\n"
	//This saves the dictionary state and enables a temporary dictionary.
	//this should prevent the eps file from polluting the vm or dictionary.
	//it's kind of the job of whatever includes EPS to do this, but this makes
	//the job easier and improves reliability. Note that in Level 2 ps, the 
	//dictionary can expand if more than 20 commands are added later.
	//If custom PS is added, it must leave the stack empty at the end.
	"save"																					"\n"
	"20 dict begin"																			"\n";
}

void output_eps_header(ostream& o, const ImageRef& s)
{
	output_eps_header(o, s.x, s.y);
}

void output_eps_footer(ostream& o)
{
	//End the temporary dictionary and restore the VM and graphics state.
	o << "end restore\n"
	//I'm not sure exactly what the end of an EPS image should be, but this 
	//seems to work.
	  << "%%EOF\n";
}


namespace PS
{

  string ps_out::bytes_to_base85(int n)
{
	//This function converts 4 raw bytes to 5 base-85 bytes. If there
	//are less than 4 bytes, then the data is zero padded.

	//Conversion to base85 is done the obvious way, but 33 is added
	//to each number to make it a printable byte.

	//There is a shortcut which means that !!!!! is abbreviated as z

	unsigned long int num = 0;
	
	for(int i=0; i <n; i++)
		num |= ((unsigned long)buf[i]) << ((3-i) * 8);
	
	if(num == 0)
		return "z";

	string r;
	r.resize(5);


	for(int i=0; i < 5; i++)
	{
		r[4-i] = (char)(num % 85 + 33);
		num /= 85;
	}

	return r;
}


void ps_out::output_header()
{
	//Output the required instructions. This sets up a 72 dpi image, the ``right'' way up, 
	//going from 0,0 - xs, ys. Since numbers are 72 per inch, the image is 72 dpi
	o << " 0 " << ys << " translate 1 -1 scale " << xs << " " << ys << " 8 matrix  currentfile\n"
		" /ASCII85Decode filter false " << m_channels << " colorimage\n";


	// <xsize> <ysize> <bits per channel> <transform> <datasrc n> ... <datasrc 1> <multi> <num of channels> colorimage
	// transform is an affine transformation matrix. We use the identity here, 
	// and preform the the transform before the image, so that any subsequent
	// graphics appear in the coordinate frame of the image.

}



ps_out::ps_out(std::ostream& out)
:o(out)
{
	num_in_buf = lines = 0;
}

ps_out::ps_out(std::ostream& out, int xsize, int ysize, int try_channels)
:o(out)
{
	xs = xsize;
	ys = ysize;
	num_in_buf = lines = 0;


	if(try_channels < 3)
		m_channels = 1;
	else
		m_channels = 3;

	output_header();

}

ps_out::~ps_out()
{
}

void ps_out::write_raw_pixel_lines(const unsigned char* data, unsigned long nlines)
{
	if((long)nlines + lines > ys)
		throw CVD::Exceptions::Image_IO::WriteError("Postscript: Internal error: attempting to write too many lines.");

	const unsigned char* end = data + (nlines * xs)*m_channels;	
	const unsigned char* d = data;
	
	string currentline;
	currentline.reserve(80);
	
	while(d < end)
	{
		//Stuff 4 bytes in to the buffer
		for(; num_in_buf < 4 && d < end; num_in_buf++, d++)
			buf[num_in_buf] = *d;
		
		//Append the base-85 encoded bytes
		currentline += bytes_to_base85(num_in_buf);
		num_in_buf = 0;

		//Output the line of it is wide enough
		if(currentline.size() >= 75)
		{
			o << currentline << endl;
			currentline.resize(0);
		}
	}
	
	//Output if there is anything left
	if(currentline.size())
		o << currentline << endl;

	lines += nlines;
	
	//Delimit the data
	if(lines == ys)
		o << "~>\n";
}

eps_out::eps_out(std::ostream& out, int xsize, int ysize, int try_channels)
:ps_out(out)
{
	xs = xsize;
	ys = ysize;

	if(try_channels < 3)
		m_channels = 1;
	else
		m_channels = 3;

	output_eps_header(o, xs, ys);
	output_header();

}

eps_out::~eps_out()
{
	output_eps_footer(o);
}

}
}
