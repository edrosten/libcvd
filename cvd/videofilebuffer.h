
#ifndef __VIDEOFILEBUFFER_H__
#define __VIDEOFILEBUFFER_H__

#include <vector>
#include <string>
#include <fstream>
#include <errno.h>

#include <cvd/videobuffer.h>
#include <cvd/videobufferflags.h>
#include <cvd/videofilebuffer_frame.h>
#include <cvd/image_io.h>

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

struct AVFormatContext;
struct AVFrame; 

namespace CVD
{
	namespace Exceptions
	{
		namespace VideoFileBuffer
		{
			struct All: public CVD::Exceptions::VideoBuffer::All { };
			struct FileOpen: public All { FileOpen(const std::string& file, const std::string& error); };
			struct BadFrameAlloc: public All { BadFrameAlloc(); };
			struct BadDecode: public All { BadDecode(double t); };
			struct EndOfFile: public All { EndOfFile(); };
			struct BadSeek: public All { BadSeek(double t); };
		}
	}

	class VideoFileBuffer : public VideoBuffer<CVD::Rgb<CVD::byte> >
	{
		public:
			VideoFileBuffer(const std::string& file);
			virtual ~VideoFileBuffer();
		
			// Base class interface
 			virtual ImageRef size()
				{return my_size;}
			virtual VideoFileFrame* get_frame();
			virtual void put_frame(VideoFrame<CVD::Rgb<CVD::byte> >* f);
			virtual bool frame_pending()
				{return frame_ready;}
			virtual void seek_to(double t);
			
			// This class additions	
			virtual void on_end_of_buffer(VideoBufferFlags::OnEndOfBuffer behaviour) 
				{end_of_buffer_behaviour = behaviour;};
		
			double frames_per_second() {return pCodecContext->frame_rate / static_cast<double>(pCodecContext->frame_rate_base);};
			std::string file_name() {return pFormatContext->filename;}
			std::string codec_name() {return pCodecContext->codec_name;}
		
		private:
			bool read_next_frame();
				
		private:
			ImageRef my_size;
			VideoBufferFlags::OnEndOfBuffer end_of_buffer_behaviour;
			double start_time;
			bool frame_ready;

			AVFormatContext* pFormatContext;
			int video_stream;
			AVCodecContext* pCodecContext;
		    AVFrame* pFrame; 
    		AVFrame* pFrameRGB;
		    CVD::Rgb<CVD::byte>* buffer;
			double frame_time;
	};

}

#endif
