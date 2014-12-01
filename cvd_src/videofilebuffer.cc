// Paul Smith 1 March 2005
// Uses ffmpeg libraries to play most types of video file


#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


#include <string>
#include <sstream>

#include <cvd/exceptions.h>
#include <cvd/videofilebuffer.h>

using namespace std;

namespace CVD
{

using namespace Exceptions::VideoFileBuffer;

namespace VFB
{


	/// Internal (non type-safe) class used by VideoFileBuffer
	/// This does the real interfacing with the ffmpeg library
	class RawVideoFileBufferPIMPL 
	{
		public:
			/// Construct a video buffer to play this file
			/// @param file The path to the video file
			/// @param is_rgb Is RGB data wanted?
			RawVideoFileBufferPIMPL(const std::string& file, bool is_rgb);
			~RawVideoFileBufferPIMPL();
		
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
				  return pCodecContext->time_base.den / static_cast<double>(pCodecContext->time_base.num);
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



//
// CONSTRUCTOR
//
RawVideoFileBufferPIMPL::RawVideoFileBufferPIMPL(const std::string& file, bool rgbp) :
	end_of_buffer_behaviour(VideoBufferFlags::RepeatLastFrame),
	pFormatContext(0),
	pCodecContext(0),
	pFrame(0), 
	pFrameRGB(0),
	//buffer(0),
	img_convert_ctx(0),
	frame_time(0.0),
	is_rgb(rgbp)
{
	try
	{
		// Register the formats and codecs
		av_register_all();
		avcodec_register_all();
	
		// Now open the video file (and read the header, if present)
		if(av_open_input_file(&pFormatContext, file.c_str(), NULL, 0, NULL) != 0)
			throw FileOpen(file, "File could not be opened.");
		
		// Read the beginning of the file to get stream information (in case there is no header)
		if(av_find_stream_info(pFormatContext) < 0)
			throw FileOpen(file, "Stream information could not be read.");
		
		// Dump details of the video to standard error
		//dump_format(pFormatContext, 0, file.c_str(), false);
		
		// We shall just use the first video stream
		video_stream = -1;
		for(unsigned int i=0; i < pFormatContext->nb_streams && video_stream == -1; i++)
		{
			
			if(pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
				video_stream = i; // Found one!
		}
		if(video_stream == -1)
			throw FileOpen(file, "No video stream found.");
		
		// Get the codec context for this video stream
		pCodecContext = pFormatContext->streams[video_stream]->codec;
		
		// Find the decoder for the video stream
		AVCodec* pCodec = avcodec_find_decoder(pCodecContext->codec_id);
		if(pCodec == NULL)
		{
			pCodecContext = 0; // Since it's not been opened yet
			throw FileOpen(file, "No appropriate codec could be found.");
		}
		
		// Open codec
		if(avcodec_open(pCodecContext, pCodec) < 0)
		{
			pCodecContext = 0; // Since it's not been opened yet
			throw FileOpen(file, string(pCodec->name) + " codec could not be initialised.");
		}
		
		// Allocate video frame
		pFrame = avcodec_alloc_frame();
		if(pFrame == NULL)
			throw BadFrameAlloc();
		
		// And a frame to hold the RGB version
		pFrameRGB = avcodec_alloc_frame();
		if(pFrameRGB == NULL)
			throw BadFrameAlloc();
		
		// How big is the buffer?
		//long num_bytes = avpicture_get_size(PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height);
		my_size = ImageRef(pCodecContext->width, pCodecContext->height);

		// And allocate a contiguous buffer
		//buffer = new CVD::Rgb<CVD::byte>[my_size.x * my_size.y];

		// Assign this buffer to image planes in pFrameRGB
		//avpicture_fill((AVPicture *)pFrameRGB, reinterpret_cast<uint8_t*>(buffer), PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height);
	
		// Now read the first frame
		if(!read_next_frame())
			throw EndOfFile();
		
		start_time = 0;
		frame_ready = true;
	}
	catch(CVD::Exceptions::All)
	{
		// Tidy things up on the heap if we failed part-way through constructing
		if(pFormatContext != 0)
		    av_close_input_file(pFormatContext);
		
		if(pCodecContext != 0)
			avcodec_close(pCodecContext);
		
		if(pFrame != 0)
			av_free(pFrame);
		
		if(pFrameRGB != 0)
			av_free(pFrameRGB);
		
		//if(buffer != 0)
		//	delete[] buffer;
		
		// Now re-throw
		throw;
	}
}

//
// DESTRUCTOR
//
RawVideoFileBufferPIMPL::~RawVideoFileBufferPIMPL()
{
    //delete [] buffer;
    av_free(pFrameRGB);
    av_free(pFrame);
    avcodec_close(pCodecContext);
    av_close_input_file(pFormatContext);
}


//
// READ NEXT FRAME
//
bool RawVideoFileBufferPIMPL::read_next_frame()
{
	uint8_t* data;

	//Make next_frame point to a new block of data, getting the sizes correct.
	//Resize always causes seperation of data.
	if(is_rgb)
	{
		next_frame_rgb.resize(my_size);
		data = reinterpret_cast<uint8_t*>(next_frame_rgb.data());
	}
	else
	{
		next_frame.resize(my_size);
		data = reinterpret_cast<uint8_t*>(next_frame.data());
	}

	//Assign this new memory block 
	avpicture_fill((AVPicture *)pFrameRGB, data, is_rgb?PIX_FMT_RGB24:PIX_FMT_GRAY8, pCodecContext->width, pCodecContext->height);


    AVPacket packet;
	av_init_packet(&packet);
	packet.stream_index = -1;
	
	// How many frames do we read looking for our video stream?
	// If we assume our streams are interlaced, and some might be interlaced
	// 2:1, this should probably do
	const int max_loop = MAX_STREAMS * 2; 
	
	int i;
	for(i = 0; packet.stream_index != video_stream && i < max_loop; i++)
	{
		
    	if(av_read_frame(pFormatContext, &packet) < 0)
			return false;

		if(packet.stream_index == video_stream)
		{
			// Ask this packet what time it is
			if(packet.pts >= 0)
				frame_time = packet.pts / static_cast<double>(AV_TIME_BASE);
			else // sometimes this is reported incorrectly, so guess
			{
				frame_time = frame_time + 1.0 / frames_per_second();
			}
			
			// Decode video frame
			int got_picture;
			#ifdef CVD_INTERNAL_FFMPEG_USE_AVCODEC_DECODE_VIDEO2
			if(avcodec_decode_video2(pCodecContext, pFrame, &got_picture, & packet) == -1)
			#else
			if(avcodec_decode_video(pCodecContext, pFrame, &got_picture, packet.data, packet.size) == -1)
			#endif
			{
				throw BadDecode(frame_time);
			}
	
			// Did we get a video frame?
			if(got_picture)
			{
				
				if(img_convert_ctx == NULL)
				{
					img_convert_ctx = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt, //Src format
													 pCodecContext->width, pCodecContext->height, is_rgb?PIX_FMT_RGB24:PIX_FMT_GRAY8, //Dest format
													 SWS_POINT, //The nastiest scaler should be OK, since we're not scaling. Right?
													 NULL, NULL, NULL);
				}

				/*
				// Convert the image from its native format to RGB
				img_convert((AVPicture *)pFrameRGB, is_rgb?PIX_FMT_RGB24:PIX_FMT_GRAY8, 
					(AVPicture*)pFrame, pCodecContext->pix_fmt, 
					pCodecContext->width, pCodecContext->height);
				*/

				sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecContext->height, pFrameRGB->data, pFrameRGB->linesize);
			}
		}

		// Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);
	}
	
	// Did we not find one?
	if(i == max_loop)
		return false;
	
	return true;
}


//
// GET FRAME
//
void* RawVideoFileBufferPIMPL::get_frame()
{

	if(!frame_pending())
		throw EndOfFile();

// 	Don't use - pCC->frame_number doesn't reset after a seek!
//  Instead, we ask the packet its time when we decode it
//	double time = start_time + pCodecContext->frame_number * pCodecContext->frame_rate_base / static_cast<double>(pCodecContext->frame_rate);

	VideoFileFrame<byte> *vfb = NULL;
	VideoFileFrame<Rgb<byte> > *vfrgb = NULL;
	
	if(is_rgb)
		vfrgb = new VideoFileFrame<Rgb<byte> >(frame_time, next_frame_rgb);
	else
		vfb = new VideoFileFrame<byte>(frame_time, next_frame);
	

	if(!read_next_frame())
	{
		switch(end_of_buffer_behaviour)
		{
			case VideoBufferFlags::RepeatLastFrame:
				// next_frame is empty because there isn't one, so 
				// I'll copy the one that I'm about to return so that
				// I can return it next time as well
				if(is_rgb)
					next_frame_rgb.copy_from(*vfrgb);
				else
					next_frame.copy_from(*vfb);
				break;
			
			case VideoBufferFlags::UnsetPending:
				frame_ready = false;
			   break;
			
			case VideoBufferFlags::Loop:
				seek_to(start_time);
				break;
		}
	}

	if(vfb)
		return vfb;
	else
		return vfrgb;
}


template<class C> void delete_frame_or_throw(void* f)
{
	VideoFrame<C>* vf = reinterpret_cast<VideoFrame<C>*>(f);
	VideoFileFrame<C>* vff  = dynamic_cast<VideoFileFrame<C>*>(vf);
	
	if(!vff)
		throw Exceptions::VideoBuffer::BadPutFrame();
	else
	        vff->delete_self();
}

//
// PUT FRAME
//
void RawVideoFileBufferPIMPL::put_frame(void* f)
{
	if(is_rgb)
		delete_frame_or_throw<Rgb<byte> >(f);
	else
		delete_frame_or_throw<byte>(f);
}

//
// SEEK TO
//
void RawVideoFileBufferPIMPL::seek_to(double t)
{	
	// The call to av_seek_frame only searches to the keyframe immediately prior to the desired frame.
	// To continue from there, we must decode one frame at a time until we reach the required frame.

	// Check that we are not seeking beyond the end of the video.
	if (t * AV_TIME_BASE + 0.5 > pFormatContext->duration)
	  throw Exceptions::VideoFileBuffer::BadSeek(t);

	// Hack: I don't know how to find the current frame number after calling av_seek_frame() without
	// calling read_frame() again. This obviously reads one extra frame, so we subtract one frame from 
	// the position we are searching for. (t = frame_number * frame_rate)
 	double frame_rate = av_q2d(pFormatContext->streams[video_stream]->r_frame_rate);
	int frame_num = static_cast<int>(t * frame_rate + 0.5);
	t = (frame_num - 1) / frame_rate;

	int64_t targetPts = static_cast<int64_t>(t * AV_TIME_BASE + 0.5);
	// If t was initially zero, it is now negative. Fix this.
	int64_t seekToPts = targetPts < 0 ? 0 : targetPts;

	// The flag AVSEEK_FLAG_ANY will seek to the specified frame, but we cannot decode from there because
	// we do not have the info from the previous frames and keyframe, hence the BACKWARD flag.
	if (av_seek_frame(pFormatContext, -1, seekToPts, AVSEEK_FLAG_BACKWARD) < 0)
	{
		cerr << "av_seek_frame not supported by this codec: performing (slow) manual seek" << endl;
		
		// Seeking is not properly sorted with some codecs
		// Fudge it by closing the file and starting again, stepping through the frames
		string file = pFormatContext->filename;
		av_close_input_file(pFormatContext);
		avcodec_close(pCodecContext);
		
		// Now open the video file (and read the header, if present)
		if(av_open_input_file(&pFormatContext, file.c_str(), NULL, 0, NULL) != 0)
		  throw FileOpen(file, "File could not be opened.");
		
		// Read the beginning of the file to get stream information (in case there is no header)
		if(av_find_stream_info(pFormatContext) < 0)
		  throw FileOpen(file, "Stream information could not be read.");
		
		// No need to find the stream--we know which one it is (in video_stream)
		
		// Get the codec context for this video stream
		pCodecContext = pFormatContext->streams[video_stream]->codec;
		
		// Find the decoder for the video stream
		AVCodec* pCodec = avcodec_find_decoder(pCodecContext->codec_id);
		if(pCodec == NULL)
		{
			pCodecContext = 0; // Since it's not been opened yet
			throw FileOpen(file, "No appropriate codec could be found.");
		}
		
		// Open codec
		if(avcodec_open(pCodecContext, pCodec) < 0)
		{
			pCodecContext = 0; // Since it's not been opened yet
			throw FileOpen(file, string(pCodec->name) + " codec could not be initialised.");
		}

		start_time = 0;
		frame_ready = true;

		// REOPENED FILE OK
	}

	// Special case
	if (targetPts < 0) {
	  read_next_frame();
	  return;
	}

	// Now read frames until we get to the time we want

	AVPacket packet;
	av_init_packet(&packet);
	int gotFrame;
	int64_t pts=0;	      
	      
	// Decode frame by frame until we reach the desired point.
	do {
		av_read_frame(pFormatContext, &packet);
		
		// Decode only video packets
		if (packet.stream_index != video_stream)
		  continue;
		
		// Timestamp of the decoded frame.
		pts = static_cast<int64_t>(packet.pts / packet.duration * AV_TIME_BASE / frame_rate + 0.5);

		#ifdef CVD_INTERNAL_FFMPEG_USE_AVCODEC_DECODE_VIDEO2
			avcodec_decode_video2(pCodecContext, pFrame, &gotFrame, &packet);
		#else
			avcodec_decode_video(pCodecContext, pFrame, &gotFrame, packet.data, packet.size);
		#endif
		av_free_packet(&packet); 
	} while (pts < targetPts);

	// This read_frame is necessary to ensure that the first frame read by the calling program
	// is actually the seeked-to frame.
	read_next_frame();
}

///Public implementation of RawVideoFileBuffer
RawVideoFileBuffer::RawVideoFileBuffer(const std::string& file, bool is_rgb, bool)
:p(new RawVideoFileBufferPIMPL(file, is_rgb))
{}

RawVideoFileBuffer::~RawVideoFileBuffer()
{}

ImageRef RawVideoFileBuffer::size()
{
	return p->size();
}

bool RawVideoFileBuffer::frame_pending()
{
	return p->frame_pending();
}

void* RawVideoFileBuffer::get_frame()
{
	return p->get_frame();
}

void RawVideoFileBuffer::put_frame(void* f)
{
	p->put_frame(f);
}

void RawVideoFileBuffer::seek_to(double t)
{
	p->seek_to(t);
}

void RawVideoFileBuffer::on_end_of_buffer(VideoBufferFlags::OnEndOfBuffer b)
{
	p->on_end_of_buffer(b);
}

double RawVideoFileBuffer::frames_per_second()
{
	return p->frames_per_second();
}

string RawVideoFileBuffer::codec_name()
{
	return p->codec_name();
}


}
} // namespace CVD
