
#ifndef CVD_VIDEOFILEBUFFER_H
#define CVD_VIDEOFILEBUFFER_H

#include <vector>
#include <string>
#include <fstream>
#include <errno.h>

#include <cvd/localvideobuffer.h>
#include <cvd/videobufferflags.h>
#include <cvd/videofilebuffer_frame.h>
#include <cvd/image_io.h>

#include <cvd/byte.h>
#include <cvd/rgb.h>

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

	namespace VFB
	{

	template<class C> struct rgb
	{
		static const bool p=Error__type_not_valid___Use_byte_or_rgb_of_byte;
	};

	template<> struct rgb<CVD::byte>
	{	
		static const bool p=false;
	};

	template<> struct rgb<CVD::Rgb<CVD::byte> >
	{	
		static const bool p=true;
	};

	class RawVideoFileBuffer 
	{
		public:
			RawVideoFileBuffer(const std::string& file, bool is_rgb);
			~RawVideoFileBuffer();
		
			// Base class interface
 			ImageRef size()
			{
				return my_size;
			}

			VideoFileFrame<byte>* get_frame();
			void put_frame(VideoFrame<CVD::byte>* f);

			bool frame_pending()
			{
				return frame_ready;
			}

			void seek_to(double t);
			
			// This class additions	
			void on_end_of_buffer(VideoBufferFlags::OnEndOfBuffer behaviour) 
			{
				end_of_buffer_behaviour = behaviour;
			}
		
			double frames_per_second() 
			{
				return pCodecContext->frame_rate / static_cast<double>(pCodecContext->frame_rate_base);
			};
			
			std::string file_name() 
			{
				return pFormatContext->filename;
			}
			
			std::string codec_name() 
			{
				return pCodecContext->codec_name;
			}
		
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
			CVD::Image<CVD::byte> next_frame;
			double frame_time;
			bool is_rgb;
	};
	}

	template<class C> class VideoFileBuffer : public CVD::LocalVideoBuffer<C>
	{
		private:
			VFB::RawVideoFileBuffer vf;
			
	
		public:
			VideoFileBuffer(const std::string& file)
			:vf(file, VFB::rgb<C>::p)
			{
			}

			~VideoFileBuffer()
			{
			}
		
			// Base class interface
			virtual ImageRef size()
			{
				return vf.size();
			}

			virtual bool frame_pending()
			{ 
				return vf.frame_pending();
			}

			virtual void on_end_of_buffer(VideoBufferFlags::OnEndOfBuffer behaviour) 
			{
				vf.on_end_of_buffer(behaviour);
			}

			virtual void seek_to(double t)
			{
				vf.seek_to(t);
			}

			virtual VideoFileFrame<C> * get_frame()
			{
				return reinterpret_cast<VideoFileFrame<C>*>(vf.get_frame());
			}

			virtual void put_frame(VideoFrame<C>* f)
			{
				vf.put_frame(reinterpret_cast<VideoFrame<byte>*>(f));
			}
			
			// This class additions	
		
			double frames_per_second() 
			{
				return vf.frames_per_second();
			}

			std::string file_name() 
			{
				return vf.file_name();
			}

			std::string codec_name() 
			{
				return vf.codec_name();
			}
		
		private:
	};

}

#endif
