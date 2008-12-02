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

#include <cvd/config.h>

extern "C" {
#ifdef CVD_INTERNAL_HAVE_FFMPEG_OLD_HEADERS
	#include <ffmpeg/avcodec.h>
	#include <ffmpeg/avformat.h>
	#include <ffmpeg/swscale.h>
#else
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
#endif
}

struct AVFormatContext;
struct AVFrame; 

namespace CVD
{
	namespace Exceptions
	{
		/// %Exceptions specific to VideoFileBuffer
		/// @ingroup gException
		namespace VideoFileBuffer
		{
			/// Base class for all VideoFileBuffer exceptions
			/// @ingroup gException
			struct All: public CVD::Exceptions::VideoBuffer::All { };
			/// Unable to open the file as a video stream, for various reasons
			/// @ingroup gException
			struct FileOpen: public All { FileOpen(const std::string& file, const std::string& error); ///< Construt from filename and error message
			};
			/// Unable to open allocate a video frame
			/// @ingroup gException
			struct BadFrameAlloc: public All { BadFrameAlloc(); };
			/// Unable to decode the video frame
			/// @ingroup gException
			struct BadDecode: public All { BadDecode(double t); ///< Construt from frame timestamp
			};
			/// get_frame() was called when at the end of the buffer
			/// @ingroup gException
			struct EndOfFile: public All { EndOfFile(); };
			/// seek_to() was called for an invalid timestamp
			/// @ingroup gException
			struct BadSeek: public All { BadSeek(double t); ///< Construt from timestamp
			};
		}
	}

	/// Internal VideoFileBuffer helpers
	namespace VFB
	{

	#ifndef DOXYGEN_IGNORE_INTERNAL
	template<class C> struct rgb
	{
		static const bool p=C::Error__type_not_valid___Use_byte_or_rgb_of_byte;
	};

	template<> struct rgb<CVD::byte>
	{	
		static const bool p=false;
	};

	template<> struct rgb<CVD::Rgb<CVD::byte> >
	{	
		static const bool p=true;
	};
	#endif 


	class A_Frame;

	/// Internal (non type-safe) class used by VideoFileBuffer
	/// This does the real interfacing with the ffmpeg library
	class RawVideoFileBuffer 
	{
		public:
			/// Construct a video buffer to play this file
			/// @param file The path to the video file
			/// @param is_rgb Is RGB data wanted?
			RawVideoFileBuffer(const std::string& file, bool is_rgb);
			~RawVideoFileBuffer();
		
			/// The size of the VideoFrames returned by this buffer
 			ImageRef size()
			{
				return my_size;
			}

			/// Returns the next frame from the buffer. This function blocks until a frame is ready.
			void* get_frame();
			/// Tell the buffer that you are finished with this frame.
			/// \param f The frame that you are finished with.
			void put_frame(void* f);

			/// Is there a frame waiting in the buffer? This function does not block. 
			bool frame_pending()
			{
				return frame_ready;
			}

			/// Go to a particular point in the video buffer (only implemented in buffers of recorded video)
			/// \param t The frame time in seconds
			void seek_to(double t);
			
			/// What should the buffer do when it reaches the end of the list of files?
			/// @param behaviour The desired behaviour
			void on_end_of_buffer(VideoBufferFlags::OnEndOfBuffer behaviour) 
			{
				end_of_buffer_behaviour = behaviour;
			}
		
			/// What is the (expected) frame rate of this video buffer, in frames per second?		
			double frames_per_second() 
			{
                        #if LIBAVCODEC_BUILD >= 4754
			  return pCodecContext->time_base.den / static_cast<double>(pCodecContext->time_base.num);
                        #else
				    return pCodecContext->frame_rate / static_cast<double>(pCodecContext->frame_rate_base);
			#endif
			};
			
			/// What is the path to the video file?
			std::string file_name() 
			{
				return pFormatContext->filename;
			}
			
			/// What codec is being used to decode this video?
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
			SwsContext *img_convert_ctx;
			
			CVD::Image<CVD::Rgb<byte> > next_frame_rgb;
			CVD::Image<CVD::byte> next_frame;
			
			double frame_time;
			bool is_rgb;
	};
	}

	///	A video buffer to play frames from a video file.
	/// This uses the ffmpeg library (http://ffmpeg.sourceforge.net/) to play
	/// a wide range of video formats, including MPEG (1, 2 and 4) and AVI (including
	/// DivX and DV) files. 
	/// Provides frames of type CVD::VideoFileFrame and throws exceptions of type
	///  CVD::Exceptions::VideoFileBuffer
	/// @param T The pixel type of the video frames. Currently only <code>CVD::Rgb<CVD::byte> ></code> and 
	/// <code>CVD::byte></code> are supported.
	/// @ingroup gVideoBuffer
	template<typename T> 
	class VideoFileBuffer : public CVD::LocalVideoBuffer<T>
	{
		private:
			VFB::RawVideoFileBuffer vf;
			
	
		public:
			/// Construct a VideoFileBuffer to play this file
			/// @param file The path to the video file
			VideoFileBuffer(const std::string& file)
			:LocalVideoBuffer<T>(VideoBufferType::NotLive),vf(file, VFB::rgb<T>::p)
			{
			}

			~VideoFileBuffer()
			{
			}
		
			virtual ImageRef size()
			{
				return vf.size();
			}

			virtual bool frame_pending()
			{ 
				return vf.frame_pending();
			}

			/// What should the buffer do when it reaches the end of the list of files?
			/// @param behaviour The desired behaviour
			virtual void on_end_of_buffer(VideoBufferFlags::OnEndOfBuffer behaviour) 
			{
				vf.on_end_of_buffer(behaviour);
			}

			virtual void seek_to(double t)
			{
				vf.seek_to(t);
			}

			virtual VideoFileFrame<T> * get_frame()
			{
				return reinterpret_cast<VideoFileFrame<T>*>(vf.get_frame());
			}

			virtual void put_frame(VideoFrame<T>* f)
			{
				vf.put_frame(f);
			}
			
			// This class additions	
		
			double frame_rate() 
			{
				return vf.frames_per_second();
			}

			/// What is the path to the video file?
			std::string file_name() 
			{
				return vf.file_name();
			}

			/// What codec is being used to decode this video?
			std::string codec_name() 
			{
				return vf.codec_name();
			}
		
		private:
	};
}

#endif
