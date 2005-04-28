/*******************************************************************************

			pnm_grok.cxx										  Ed Rosten 2003

This file provides very basic functionality for accessing PNM images. PNM images
are either greyscale (PBMs are treated as PGMs of depth 1) or RGB, and either 1 
or 2 bytes per element. 

Images are dealt with by lines. 1 byte images are delt with as lines of
unsigned chars and 2 byte images are dealt with as lines of unsigned shorts.
Luma images are stored as an array on luma pixel values and RGB images are
dealt with as RGBRGBRGB... packed in to an array. The base class provides the
method elements_per_line() which returns the length of the array needed to
store a single line of the image.

The library here provides no image re-encoding. A 2 byte colour image can only
be read as a 2 byte colour image. The exception is that a PBM is read as a PGM 
of depth 1.

In case of an error, an exception of type err is thrown. The error codes are
arranged in to errors to do with the PNM images and things probably resulting
from a logic error in the calling program.

*******************************************************************************/


#include <iostream>
#include <ctype.h>
#include <limits>

#include "pnm_src/pnm_grok.h"
#include <cvd/config.h>
#include <cvd/image_io.h>

/* Compile time, arch specific parameters

PNMs are big endian, so for them to be loaded quickly (ie using fwrite), the
bytes have to be swapped on little endian machines. This is OK for loading
since the byte swapping can be done in place, but not OK for saving since the
array of data is passed as const, so it can't be swapped in place.

Short and long PNMs are the same thing. they're 2 byte PNMs (long because
they're longer than  byte PNMs and short since they fit in to the "short" 
data type).
*/

//#ifdef FAST_SHORT_IO
	#define LONG_PNM_FAST_LOAD

	#ifdef ARCH_LITTLE_ENDIAN
		#define SWAP_BYTES
	#else
		#define LONG_PNM_FAST_SAVE
	#endif
//#endif

#define PBM 0
#define PGM 1
#define PPM 2

using namespace std;

namespace CVD
{
namespace PNM
{

//All possible error codes. These are loosely divided in to data errors (ie
//problems with the input image) and programmer errors (runtime errors
//which are almost definitely caused by some logic error in the program).
enum errors
{
	E_NONE = 0,					//Data errors
	E_NOT_PNM,				
	E_HEOF,				
	E_DEOF,	
	E_BAD_SIZE,
	E_BAD_MAXVAL,
	E_PBM_NOT_IMPLEMENTED,
	E_OUT_OF_MEMORY,
	E_UNTERMINATED_HEADER,
	E_PAM_NOT_IMPLEMENTED,
	E_LAST_DATA_ERROR,

	E_PROGRAMMER_ERROR = 255,	//All data errors are sit below this

	
								//Probably programmer errors:
	E_NO_LONG_PBM,
	E_WRITE_PAST_END,
	E_NOT_2_BYTE,
	E_NOT_1_BYTE,
	E_READ_PAST_END,

	E_MAX						//All errors sit below this
};

string name_error(errors e)
{
	const char* data_error_names[]=
	{
		"None.",
		"Not a PNM.",
		"EOF in header.",
		"EOF in data.",
		"Bad size specified.",
		"Invalid colour MAXVAL.",
		"Binary PBMs are not implemented.",
		"Out of memory.",
		"Unterminated header.",
		"Portable Arbirtary Maps are not implemented.",
		"Last data error. This should *never* occur."
	};
		
	const char* prog_error_names[]=
	{
		"Long PBMs are silly.",
		"Trying to write past end of file.",
		"This is not a 2 byte PNM.",
		"This is not a 1 byte PNM.",
		"Trying to read past the end of the file."
	};

	const char* invalid = "Not a valid error.";

	string ret;

	if(e < 0 || e >= E_MAX)
		ret=string("internal error: ") + invalid;
	else if(e >= E_LAST_DATA_ERROR && e <= E_PROGRAMMER_ERROR)
		ret=string("internal error: ") + invalid;
	else if(e < E_LAST_DATA_ERROR)
		ret=data_error_names[e];
	else
		ret=string("internal error: ") + prog_error_names[e-E_PROGRAMMER_ERROR - 1];

	return "Error in PNM image: " + ret;
}




bool clean(istream& i)
{
	////////////////////////////////////////////////////////////////////////////
	//
	// Strip leading whitespae and PNM comments from an istream
	// comments start with a '#' and and with a newline

	unsigned char  c;

	for(;;)
	{
		i >> c;

		if(i.eof()) //I'm pedantic like that.
			return 0;
		else if(c == '#') //Eat a whole line of comment
		{
			do
			{
				i >> c;
				if(i.eof())	//I'm still a pedant
					return 0;
			}while(c != '\n');
		}
		else if(!isspace(c))
		{
			i.putback(c);
			return 1;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// base PNM class definitions
//
////////////////////////////////////////////////////////////////////////////////

bool pnm_out::can_proc_lines(unsigned long nl)
{
	//Slightly oddly named. Update the number of processed lines and return 
	//an error condition if the lines can not be processed.

	lines_so_far += nl;
	if(lines_so_far > ys)
		return false;
	else
		return true;
}

bool pnm_in::can_proc_lines(unsigned long nl)
{
	//Slightly oddly named. Update the number of processed lines and return 
	//an error condition if the lines can not be processed.

	lines_so_far += nl;
	if(lines_so_far > ys)
		return false;
	else
		return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// Input PNM class definitions
//
////////////////////////////////////////////////////////////////////////////////

pnm_in::pnm_in(std::istream& in)
:i(in)
{
	lines_so_far=0;
	read_header();
}

void pnm_in::read_header()
{
	//Fix the stream before returning.
	#define RETURN(X) do{										\
						i >> skipws;							\
						if(X == E_NONE)							\
							return;								\
						else									\
							throw Exceptions::Image_IO::MalformedImage(name_error(X));\
					  }while(0)									

	#define CLEAN 	do{											\
						if(!clean(i))							\
							throw Exceptions::Image_IO::MalformedImage(name_error(E_HEOF));\
					}while(0)

	#define GET(X) 	do{											\
						CLEAN;									\
						i>>X;									\
						if(i.eof())								\
							throw Exceptions::Image_IO::MalformedImage(name_error(E_HEOF));\
					}while(0)

	char c1, c2;
	
	// i >> (int) will eat whitespace before and after the integer. This breaks
	// reading maxval since it can (and does) eat in to the image.
	i >> noskipws;


	//Read and check magic number

	i >> c1 >> c2;
	if(i.eof())
		RETURN(E_HEOF);

	if(c1 != 'P' || c2 < '1' || c2 > '7')
		RETURN(E_NOT_PNM);

	if(c2 == '7')
		RETURN(E_PAM_NOT_IMPLEMENTED);

	if(strchr("123", c2))
		is_text = true;
	else
		is_text = false;

	if(c2 == '1' || c2 == '4')
		type = PBM;
	else if(c2 == '2' || c2 == '5')
		type = PGM;
	else
		type = PPM;


	if(type == PGM)
		m_channels = 1;
	else 
		m_channels = 3;

	//Read and check image dimensions
	GET(xs);
	GET(ys);

	if(xs <= 0 || ys <= 0)
		RETURN(E_BAD_SIZE);
	
	//Read if necessasy and set MAXVAL
	if(type != PBM)
	{	
		GET(maxval);

		if(maxval <= 0 || maxval > 65535)
			RETURN(E_BAD_MAXVAL);
				
		if(maxval <= 255)
			m_is_2_byte = false;
		else
			m_is_2_byte = true;
	}
	else
	{
		maxval = 255;
		m_is_2_byte = false;
	}
	
	//Remove everything to the beginning of the image
	if(is_text)
		CLEAN;
	else
	{
		//Binary PNMs have a single byte of whitespace.
	
		unsigned char tmp;
		i >> tmp;
		if(!isspace(tmp))
			RETURN(E_UNTERMINATED_HEADER);
	}

//hee hee hee. This makes the SGI compiler segfault :-)
//	RETURN(E_NONE);
	i >> skipws;

//HACK
//cerr << "is_rgb=" << m_is_rgb <<". xsize=" << xs << ". ysize=" << ys << ". is_text=" << is_text << ". type=" << type  << ". maxval=" << maxval << endl; 

	
	return;
	#undef RETURN
	#undef CLEAN
	#undef GET
}

void pnm_in::get_raw_pixel_lines(unsigned char* c, unsigned long nlines)
{
	//Reading into uchars is sufficiently different from reading in to
	//ushorts, so it has ben done as 2 functions as opposed to one 
	//tmeplated one.

	#define RETURN(X) do{										\
						if(X == E_NONE)							\
							return;								\
						else									\
							throw Exceptions::Image_IO::MalformedImage(name_error(X));\
					  }while(0)									

	unsigned long k, j, npix;
	unsigned char* cc = c;

	if(!can_proc_lines(nlines))
		RETURN(E_READ_PAST_END);
	
	//Load a bunch of pixels in to memory without doing internal buffering
	
	npix = xs * nlines;

	if(m_is_2_byte)
		RETURN(E_NOT_1_BYTE);
	
	if(type == PPM)
		npix *= 3;
	
	if(is_text)		//Load text PNM data
	{
		//Don't report errors. The spec says to be really leniant with
		//respect to errors in text only pnms

		for(k=0; k < npix; k++)
		{
			i >> j;	

			if(i.eof())
				j=0;

			*c++ = j & 0xff;
		}

		if(type == PBM)
		{
			//PBMs are inverted. Also, converto to a sensible maxval
			for(k=0; k < npix; k++, cc++)
			{
				if(*cc)
					*cc=0;
				else
					*cc=maxval;
			}
		}
	}
	else if(type != PBM)
	{
		i.read((char*)c, (int)npix);
	}
	else
		RETURN(E_PBM_NOT_IMPLEMENTED);

	if(i.eof())
		RETURN(E_DEOF);
}


void pnm_in::get_raw_pixel_lines(unsigned short* s, unsigned long nlines)
{
	unsigned long k, j, npix;
	
	//Load a bunch of pixels in to memory without doing internal buffering
	
	if(!can_proc_lines(nlines))
		RETURN(E_READ_PAST_END);

	if(!m_is_2_byte)
		RETURN(E_NOT_2_BYTE);

	npix = xs * nlines;
	
	if(type == PPM)
		npix *= 3;
	
	if(is_text)		//Load text PNM data
	{
		//Don't report errors. The spec says to be really leniant with
		//respect to errors in text only pnms

		for(k=0; k < npix; k++)
		{
			i >> j;	
			if(i.eof())
				j=0;
			*s++ = j & 0xffff;
		}
	}
	else
	{
		#ifndef LONG_PNM_FAST_LOAD

			unsigned char i1, i2;

			for(j=0; j < npix; j++)
			{
				i.read((char*)&i1, 1);
				i.read((char*)&i2, 1);

				//Data is big-endian
				*s++ = i1 << 8 | i2;

				if(i.eof())
					cerr << j << " ";
			}
		#else
			i.read((char*)s, npix*2);
				
			#ifdef SWAP_BYTES
				unsigned char t;

				for(j=0; j < npix; j++, s++)
				{
					t = *(unsigned char*)s;
					*(unsigned char*)s = *(1+(unsigned char*)s);
					*(1+(unsigned char*)s) = t;
				}
			#endif
		#endif
	}

	if(i.eof())
		RETURN(E_DEOF);
}
////////////////////////////////////////////////////////////////////////////////
//
// Output PNM class definitions
//
////////////////////////////////////////////////////////////////////////////////

pnm_out::pnm_out(std::ostream& out, int xsize, int ysize, int try_channels, bool use2bytes, const std::string& comm)
:o(out)
{
	lines_so_far=0;
	xs=xsize;
	ys=ysize;

	if(try_channels < 3)
		m_channels = 1;
	else
		m_channels = 3;

	if(m_channels == 3)
		type = PPM;
	else 
		type = PGM;


	m_is_2_byte = use2bytes;
	if(use2bytes)
		maxval = 0xffff;
	else
		maxval = 0xff;
	
	is_text=false;
	
	write_header(comm);
}

void pnm_out::write_header(const std::string& comments)
{
	//Figure out which magic number to write
	unsigned char m[3]="P ";
	
	if(type == PGM)
		m[1] = '2';
	else // (type == PPM)
		m[1] = '3';
	
	if(!is_text)
		m[1] += 3;

	//Write comments
	o << m <<"\n" 
	  << "# Created by Ed's cool PNM grokking library in CVD\n"
	  << "# CVD Version " << CVD_MAJOR_VERSION << "." << CVD_MINOR_VERSION << "\n";

	//Indent commants with 1 space. Ensure embedded newlines do the right thing
	//Make sure comments (if they exist) are terminated with a newline.
	
	unsigned int i;
	bool last_was_nl = true;

	for(i=0; i < comments.size(); i++)
	{
		if(last_was_nl)
		{
			o << "# ";
			last_was_nl = false;
		}
	
		o << comments[i];

		if(comments[i] == '\n')
			last_was_nl = true;
	}
	if(!last_was_nl)
		o << "\n";

	//Write the rest of the pixmap information
	o << xs << " " << ys << "\n";
	o << maxval << "\n";
}


void pnm_out::write_raw_pixel_lines(const unsigned char* mem, unsigned long nlines)
{
	unsigned long npix = nlines * xs, j;

	if(!can_proc_lines(nlines))
		RETURN(E_WRITE_PAST_END);

	if(type == PPM)
		npix*=3;

	if(m_is_2_byte)
		RETURN(E_NOT_1_BYTE);
	if(is_text)
	{
		for(j=0; j < npix; j++, mem++)
			o << (unsigned int)*mem << "\n";
	}
	else
		o.write((const char*)(mem), npix);
}


void pnm_out::write_raw_pixel_lines(const unsigned short* mem, unsigned long nlines)
{
	unsigned long npix = nlines * xs, j;

	if(!can_proc_lines(nlines))
		RETURN(E_WRITE_PAST_END);


	if(type == PPM)
		npix*=3;

	if(!m_is_2_byte)
		RETURN(E_NOT_2_BYTE);	
	else if(is_text)
	{
		for(j=0;  j < npix; j++)
			o << *mem++ << "\n";
	}
	else
	{
		#ifndef LONG_PNM_FAST_SAVE
		
		unsigned char il, ih;
		for(j=0; j < npix; j++)
		{
			il = *mem & 0xff;
			ih = (*mem++ >> 8) & 0xff;

			o << ih << il;
		}

		#else
			o.write((char*)mem, npix*2);
		#endif
	}
}

}
}
