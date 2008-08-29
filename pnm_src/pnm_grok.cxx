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

#include <cvd/internal/io/pnm_grok.h>
#include <cvd/internal/load_and_save.h>
#include <cvd/config.h>

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

#ifdef CVD_ARCH_LITTLE_ENDIAN
#define SWAP_BYTES
#else
#define LONG_PNM_FAST_SAVE
#endif
//#endif

#define PBM 0
#define PGM 1
#define PPM 2

using namespace std;
using namespace CVD::Exceptions::Image_IO;

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
#define RETURN(X) do{							\
		i >> skipws;						\
		if(X == E_NONE)						\
		    return;						\
		else							\
		    throw Exceptions::Image_IO::MalformedImage(name_error(X)); \
	    }while(0)									

#define CLEAN 	do{							\
		if(!clean(i))						\
		    throw Exceptions::Image_IO::MalformedImage(name_error(E_HEOF)); \
	    }while(0)

#define GET(X) 	do{							\
		CLEAN;							\
		i>>X;							\
		if(i.eof())						\
		    throw Exceptions::Image_IO::MalformedImage(name_error(E_HEOF)); \
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

#define RETURN(X) do{							\
		if(X == E_NONE)						\
		    return;						\
		else							\
		    throw Exceptions::Image_IO::MalformedImage(name_error(X)); \
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

	void writePNMHeader(ostream& out, int channels, ImageRef size, int maxval, bool text, const std::string& comments)
	{
	    char m[3] = {'P',' ', '\n'};
	    if (channels == 1)
		m[1] = '2';
	    else 
		m[1] = '3';
	    if (!text)
		m[1] += 3;
	    out.write(m, 3);
	    bool freshLine = true;
	    for (size_t i=0; i<comments.length(); i++) {
		if (freshLine)
		    out << "# ";
		char c = comments[i];
		if (c == '\n') {
		    freshLine = true;
		    out << endl;
		} else {
		    out << c;
		    freshLine = false;
		}
	    }
	    if (!freshLine)
		out << endl;
	    out << size.x << " " << size.y << endl << maxval << endl;
	}

	pnm_writer::pnm_writer(std::ostream& out, ImageRef size_, const std::string& type_)
	:row(0),o(out),size(size_),type(type_)
	{
		
		if(type == "unsigned char")
			writePNMHeader(out, 1, size, 255, 0, "");
		else if(type == "unsigned short")
			writePNMHeader(out, 1, size, 65535, 0, "");
		else if(type == "CVD::Rgb<unsigned char>")
			writePNMHeader(out, 3, size, 255, 0, "");
		else if(type == "CVD::Rgb<unsigned short>")
			writePNMHeader(out, 3, size, 65535, 0, "");
		else
		    throw UnsupportedImageSubType("PNM", type);
	}

	pnm_writer::~pnm_writer()
	{}

	void pnm_writer::write_shorts(const unsigned short* data, int n)
	{
	  
		#ifdef CVD_ARCH_LITTLE_ENDIAN
			const unsigned char* bdata = (const unsigned char*)data;
			for (int i=0; i<n; i++, bdata+=2) 
			{
				unsigned char lohi[2] = {bdata[1], bdata[0]};
				o.write((const char*)lohi,2);
			}
		#else
			o.write((const char*)data, n*sizeof(unsigned short));
		#endif
	}

	template<class P> void pnm_writer::sanity_check(const P*)
	{
	    if(type != PNM::type_name<P>::name())
			throw WriteTypeMismatch(type, PNM::type_name<P>::name());
	
	    //Do some sanity checking
	    if(row >= size.y)
			throw InternalLibraryError("CVD", "Write past end of image.");
	
	    row++;
	}

	void pnm_writer::write_raw_pixel_line(const unsigned char* data)
	{
		sanity_check(data);
		o.write(reinterpret_cast<const char*>(data), size.x);
	}

	void pnm_writer::write_raw_pixel_line(const unsigned short* data)
	{
		sanity_check(data);
		write_shorts(data, size.x);
	}

	void pnm_writer::write_raw_pixel_line(const Rgb<unsigned char>* data)
	{
		sanity_check(data);
		o.write(reinterpret_cast<const char*>(data), 3*size.x);
	}

	void pnm_writer::write_raw_pixel_line(const Rgb<unsigned short>* data)
	{
		sanity_check(data);
		write_shorts(reinterpret_cast<const unsigned short*>(data), 3*size.x);
	}


/*
	template <class T> void writePNMPixelsText(ostream& out, const T* data, size_t count) 
	{
	    size_t lines = count / 25;
	    size_t k=0;
	    for (size_t i=0; i<lines; i++) {
		for (size_t j=0; j<25; j++)
		    out << (int)(data[k++]) << " ";
		out << endl;
	    }
	    while (k<count)
		out << (int)(data[k++]);
	    out << endl;
	}
*/

    }
}
