#include "pnm_src/jpeg.h"

#include "cvd/image_io.h"
using namespace std;
#include <iostream>
#include <iomanip>
#include <setjmp.h>

namespace CVD
{
namespace PNM
{

struct jpeg_istream_src: public jpeg_source_mgr
{

	istream* i;
	bool eof;
	static const int bufsize=8192;
	JOCTET buf[bufsize+2];

	//Constructor
	static void create(j_decompress_ptr p, istream* is)
	{
		//Allocate memory
		p->src = (jpeg_istream_src*) (*p->mem->alloc_small)((jpeg_common_struct*)p, JPOOL_PERMANENT, sizeof(jpeg_istream_src));

		jpeg_istream_src* me = (jpeg_istream_src*)p->src;

		//Set up virtual member functions
		me->init_source = s_init_src;
		me->fill_input_buffer = s_fill_input_buffer;
		me->skip_input_data = s_skip_input_data;
		me->resync_to_restart = jpeg_resync_to_restart;
		me->term_source = s_term_source;

		//Set up data members
		me->i = is;
		me->bytes_in_buffer = 0;
		me->next_input_byte = me->buf;
		me->eof = 0;
	}
	

	static void s_init_src(j_decompress_ptr p)
	{
		jpeg_istream_src* me = (jpeg_istream_src*)p->src;

		me->bytes_in_buffer = 0;	
	}

	static boolean s_fill_input_buffer(j_decompress_ptr p)
	{

		int n=0;
		jpeg_istream_src* me = (jpeg_istream_src*)p->src;
		me->next_input_byte = me->buf;

		if(me->eof)
		{
			//Output JPEG end-of-image flags
			me->buf[0] = 0xff;
			me->buf[1] = JPEG_EOI;
			me->bytes_in_buffer = 2;
			return true;
		}

		int c;
		for(n=0; n < bufsize;  n++)
		{
			//Get a byte...
			c = me->i->get();
			

			//Check for EOF...
			if(c  == EOF)
			{
				me->eof = 1;
				break;
			}
			
			//Store the byte...
			me->buf[n] = c;
			
			//ooooh! a marker!
			if(c == 0xff)
			{
				c = me->i->get();
				if(c == EOF)
				{
					me->eof = 1;
					break;
				}


				me->buf[++n] = c;

				if(c == JPEG_EOI)
				{
					me->eof = 1;
					break;
				}
			}
		}

		//me->i->read((char*)(me->buf), bufsize);
		//me->bytes_in_buffer = me->i->gcount();	
		me->bytes_in_buffer = n;

		if(me->i->eof())
			me->eof = 1;

		if(me->bytes_in_buffer == 0)
			s_fill_input_buffer(p);

		return true;
	}

	static void s_skip_input_data(j_decompress_ptr p, long num)
	{
		jpeg_istream_src* me = (jpeg_istream_src*)p->src;


		if(num > (long)(me->bytes_in_buffer))
		{
			me->i->ignore(num - me->bytes_in_buffer);
			me->bytes_in_buffer = 0;

		}

		else
		{
			me->next_input_byte += num;
			me->bytes_in_buffer -= num;
		}
	}

	static void s_term_source(j_decompress_ptr p)
	{
	}
};


void jumpy_error_exit (j_common_ptr cinfo)
{
	//Exceptions don't work from C code, so use setjmp/longjmp to jump back
	//in to C++ code.
	longjmp(*(jmp_buf*)(cinfo->client_data), 1);
}

struct jpeg_error_mgr * jumpy_error_manager (struct jpeg_error_mgr * err)
{
	//Set up most of the useful defaults
	jpeg_std_error(err);
	//Except we don't want to exit on an error.
	err->error_exit = jumpy_error_exit;
	return err;
}

jpeg_in::jpeg_in(istream& in)
:i(in)
{
	cinfo.err = jumpy_error_manager(&jerr);

	jmp_buf env;
	cinfo.client_data = &env;

	//Catch "exceptions" and throw proper exceptions
	if(setjmp(env))
	{
		//longjmp called
		char buffer[JMSG_LENGTH_MAX];
		(cinfo.err->format_message)((jpeg_common_struct*)&cinfo, buffer);
		throw CVD::Exceptions::Image_IO::MalformedImage(string("Error in JPEG image: ") + buffer);
	}
	
	jpeg_create_decompress(&cinfo);
	jpeg_istream_src::create(&cinfo, &i);

	jpeg_read_header(&cinfo, true);
	jpeg_start_decompress(&cinfo);

	xs = cinfo.output_width;
	ys = cinfo.output_height;

	if(cinfo.out_color_components == 3)
		m_is_rgb = 1;
	else
		m_is_rgb = 0;

	m_is_2_byte = 0;
}

void jpeg_in::get_raw_pixel_lines(unsigned char*data, unsigned long nlines)
{
	jmp_buf env;
	cinfo.client_data = &env;
	
	//Catch "exceptions" and throw proper exceptions
	if(setjmp(env))
	{
		//longjmp called
		char buffer[JMSG_LENGTH_MAX];
		(cinfo.err->format_message)((jpeg_common_struct*)&cinfo, buffer);
		throw CVD::Exceptions::Image_IO::MalformedImage(string("Error in JPEG image: ") + buffer);
	}

	
	unsigned char** datap = &data;
	for(unsigned int i=0; i < nlines; i++)	
	{
		jpeg_read_scanlines(&cinfo, datap, 1);
		data += elements_per_line();
	}
}

void jpeg_in::get_raw_pixel_lines(unsigned short*datap, unsigned long nlines)
{
}

jpeg_in::~jpeg_in()
{
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
}


}
}
