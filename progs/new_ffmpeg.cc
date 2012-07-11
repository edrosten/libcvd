/*                       
	This file is part of the CVD Library.

	Copyright (C) 2012 E. Rosten

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

//A mix of videofilebuffer.cc and documentation from http://plagatux.es/2011/07/using-libav-library/

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <string>
#include <iostream>
#include <cvd/image_io.h>

using namespace std;
using namespace CVD;

#define DS(X) do{if(debug) cerr << "VideoFileBuffer2: " << X << endl;} while(0)
#define DV(X,Y) do{if(debug) cerr << "VideoFileBuffer2: " << X << " = " << Y << endl;} while(0)
#define Dv(X) do{if(debug) cerr << "VideoFileBuffer2: " << #X << " = " << (X) << endl;} while(0)
#define DR(X) do{if(debug) cerr << "VideoFileBuffer2: " << #X << " returned " << r << endl;} while(0)
namespace CVD{

namespace Exceptions
{
	struct VideoFileBuffer2
	{
		string what;
		VideoFileBuffer2(string s_)
		:what(s_)
		{}
	};
}

template<class T>
struct PixFmt
{
};

template<>
struct PixFmt<byte>
{
	static PixelFormat get()
	{
		return PIX_FMT_GRAY8;
	}
};

template<>
struct PixFmt<Rgb<byte> >
{
	static PixelFormat get()
	{
		return PIX_FMT_RGB24;
	}
};

class VideoFileBuffer2
{
	bool rgb;
	PixelFormat             output_fmt;
	AVFormatContext *       input_format_context;
	struct SwsContext *     image_converter_context;

	int                     video_stream_index;
	AVCodecContext *        video_codec_context;

	AVFrame *               raw_image;
	AVFrame *               converted_image;

	uint8_t *               video_buffer;
	int                     video_buffer_size;

	AVPacket                packet;

	ImageRef                size;
	unsigned int            frame_number;
	bool                    eof;
	double                  frame_rate;
	
	AVCodec*                video_codec;
	SwsContext*             img_convert_context;

	bool                    debug;

	Image<Rgb<byte> >       rgb_frame;
	Image<byte>             mono_frame;

	void die(int e)
	{
		if(e < 0)
		{
			char buf[1024];
			av_strerror(e, buf, sizeof(buf)-1);
			buf[sizeof(buf)-1] = 0;
			
			throw Exceptions::VideoFileBuffer2(buf);
		}
	}

	public:

	VideoFileBuffer2(const string& fname, bool rgb_, bool debug_)
	:rgb(rgb_),
	 output_fmt(rgb?PixFmt<Rgb<byte> >::get():PixFmt<byte>::get()),
	 input_format_context(0),
	 image_converter_context(0),
	 video_stream_index(-1),
	 video_codec_context(0),
	 raw_image(0),
	 converted_image(0),
	 video_buffer(0),
	 video_buffer_size(-1),
	 frame_number(0),
	 eof(false),
	 video_codec(0),
	 img_convert_context(0),
	 debug(debug_)
	{

		try
		{ 
			av_register_all();
			DS("starting");

			int r;
			string err;

			input_format_context = avformat_alloc_context();
			if(input_format_context == NULL)
				throw Exceptions::VideoFileBuffer2("Out of memory.");

			//avformat_open_input semes to be the latest non-depracated method for this task.
			//It requires a pre-allocated context.
			r = avformat_open_input(&input_format_context, fname.c_str(), NULL, NULL);
			DR(av_open_input_file);
			die(r);


			r = avformat_find_stream_info(input_format_context, NULL);
			DR(av_find_stream_info);
			die(r);
			
			DV("number of streams", input_format_context->nb_streams);
			DS("Enumerating streams:");
			video_stream_index = -1;
			//Trawl through the various streams to get some nice
			//verbosity and to find the first video stream.
			for (unsigned int i=0; i< input_format_context->nb_streams; i++)
			{
				AVMediaType t = input_format_context->streams[i]->codec->codec_type;
				AVCodecContext* vc = input_format_context->streams[i]->codec;

				if(t == AVMEDIA_TYPE_UNKNOWN)
					DS("    " << i << ": " << "unknown");
				else if(t == AVMEDIA_TYPE_VIDEO)
				{
					DS("    " << i << ": " << "video");

					if(video_stream_index != -1)
					{
						DS("        warning, multiple video streams.");
					}
					else
					{
						DS("        selecting this stream.");
						video_stream_index=i;

						video_codec_context = input_format_context->streams[i]->codec;
						size.x = video_codec_context->width;
						size.y = video_codec_context->height;
					}



					// Add video stream (see next section)
					DV("        width             ", vc->width);
					DV("        height            ", vc->height);
					DV("        bit rate          ", vc->bit_rate);
					DV("        timebase (num)    ", vc->time_base.num);
					DV("        timebase (denom)  ", vc->time_base.den);
					DV("        ticks per frame   ", vc->ticks_per_frame);

					//Frame timestamps are represented as integer multiples of
					//rationals with the obvious definition.  The number of
					//incrementes between frames is not necessarily 1, so this
					//number cannot be used to compute the frame rate.

					//The number of ticks per frame gives the frame rate.
					frame_rate = 1. / ( 1.0 * vc->ticks_per_frame*  vc->time_base.num / vc->time_base.den);
					DV("        frame rate (?)    ", frame_rate);

					//The ticks is usually 2 for interlaced video and 1 otherwise.
					//However, some progressive video converted from interlaced also has
					//ticks=2


				}
				else if(t == AVMEDIA_TYPE_AUDIO)
					DS("    " << i << ": " << "audio");
				else if(t == AVMEDIA_TYPE_DATA)
					DS("    " << i << ": " << "data");
				else if(t == AVMEDIA_TYPE_SUBTITLE)
					DS("    " << i << ": " << "subtitle");
				else if(t == AVMEDIA_TYPE_ATTACHMENT)
					DS("    " << i << ": " << "attachment");

				//For some reason vc->codec_name is empty.
				//This seems to work.
				char buf[1024];
				avcodec_string(buf, sizeof(buf)-1, vc, false);
				buf[sizeof(buf)-1]=0;
				DV("        codec name        ", buf << "*"); 

			}

			if(video_stream_index == -1)
				throw Exceptions::VideoFileBuffer2("VideoFileBuffer2: no video stream found.");
			
			//Get hold of the video decoding mechanism
			video_codec = avcodec_find_decoder(video_codec_context->codec_id);
			if(video_codec == NULL)
				throw Exceptions::VideoFileBuffer2("VideoFileBuffer2: no decoder found");

			r = avcodec_open2(video_codec_context, video_codec, 0);
			DR(avcodec_open2);
			die(r);

			raw_image = avcodec_alloc_frame();

			//Get hold of the mechanism to convert the video frame into
			//a useful pixel format. The sws part is GPL, not LGPL.
			//
			//In principle, this could be done using libCVD's internal conversion
			//code in order to break the GPL dependence.
			converted_image= avcodec_alloc_frame();
			img_convert_context  = sws_getContext(size.x, size.y, video_codec_context->pix_fmt, //Input
			                                      size.x, size.y, output_fmt,                   //Output
			                                      SWS_POINT, //The nastiest scaler should be OK, since we're not scaling. Right? right?
												  NULL, NULL, NULL);

			if(img_convert_context == 0)
				throw Exceptions::VideoFileBuffer2("Software scaler not found! This is very strange.");


			return;
		}
		catch(Exceptions::VideoFileBuffer2)
		{
			free_all();
			throw;
		}
	}

	template<typename T>
	Image<T> get_next_frame()
	{

		if(PixFmt<T>::get() != output_fmt)
			throw Exceptions::VideoFileBuffer2("Mismatched format: probably an internal library error.");

		Image<T> ret;


		while(ret.size().area()== 0)
		{
			av_init_packet(&packet);
			int r = av_read_frame(input_format_context, &packet);	
			DR(av_read_frame);

			if(r == AVERROR_EOF)
			{
				DS("EOF");
			}
			die(r);

			Dv(packet.stream_index);

			if(packet.stream_index == video_stream_index)
			{
				int got_picture;
				avcodec_decode_video2(video_codec_context, raw_image, &got_picture, &packet);
				Dv(got_picture);

				if(!got_picture)
					goto cont;
			
				//Allocate memory for the converted image
				ret.resize(size);

				//Set up converted_image so it uses the data in ret as its data buffer.
				avpicture_fill((AVPicture*)converted_image, reinterpret_cast<uint8_t*>(ret.data()), output_fmt, size.x, size.y);
				sws_scale(img_convert_context, raw_image->data, raw_image->linesize, 0, size.y, converted_image->data, converted_image->linesize);
			}
			
			cont:

			//Free the data owned by packet, not the struct tiself
			av_free_packet(&packet);
		}
		
	
		return ret;
	}

	~VideoFileBuffer2()
	{
		free_all();
	}		
	
	void free_all()
	{
		if(input_format_context != NULL)
			avformat_free_context(input_format_context);

		if(raw_image != NULL)
			av_free(raw_image);

		if(converted_image != NULL)
			av_free(converted_image);

		if(img_convert_context != NULL)
			sws_freeContext(img_convert_context);
	}

};
}

int main(int, char** argv)
{
	try{
		VideoFileBuffer2 foo(argv[1], true, true);

		for(;;)
		{
			Image<Rgb<byte> > pic = foo.get_next_frame<Rgb<byte> >();
			img_save(pic, "foo.jpg");
		}
	}

	catch(Exceptions::VideoFileBuffer2 e)
	{
		cerr << "Error: " << e.what << endl;

	}

}
