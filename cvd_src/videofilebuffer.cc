// Paul Smith 1 March 2005
// Uses ffmpeg libraries to play most types of video file

#include <string>
#include <sstream>

#include <cvd/exceptions.h>
#include <cvd/videofilebuffer.h>

using namespace std;

namespace CVD
{

using namespace Exceptions::VideoFileBuffer;
	
//
// EXCEPTIONS
//

Exceptions::VideoFileBuffer::FileOpen::FileOpen(const std::string& name, const string& error)
{
	what = "VideoFileBuffer: Error opening file \"" + name + "\": " + error;
}

Exceptions::VideoFileBuffer::BadFrameAlloc::BadFrameAlloc()
{
	what = "VideoFileBuffer: Unable to allocate video frame.";
}

Exceptions::VideoFileBuffer::BadDecode::BadDecode(double t)
{
	ostringstream os;
	os << "VideoFileBuffer: Error decoding video frame at time " << t << ".";
	what = os.str();
}

Exceptions::VideoFileBuffer::EndOfFile::EndOfFile()
{
	what =  "VideoFileBuffer: Tried to read off the end of the file.";
}

Exceptions::VideoFileBuffer::BadSeek::BadSeek(double t)
{
	ostringstream ss;
	ss << "VideoFileBuffer: Seek to time " << t << "s failed.";
	what = ss.str();
}

//
// CONSTRUCTOR
//
VideoFileBuffer::VideoFileBuffer(const std::string& file) :
	end_of_buffer_behaviour(VideoBufferFlags::RepeatLastFrame),
	pFormatContext(0),
	pCodecContext(0),
	pFrame(0), 
	pFrameRGB(0),
	buffer(0),
	frame_time(0.0)
{
	try
	{
		// Register the formats and codecs
		av_register_all();
	
		// Now open the video file (and read the header, if present)
		if(av_open_input_file(&pFormatContext, file.c_str(), NULL, 0, NULL) != 0)
			throw FileOpen(file, "File could not be opened.");
		
		// Read the beginning of the file to get stream information (in case there is no header)
		if(av_find_stream_info(pFormatContext) < 0)
			throw FileOpen(file, "Stream information could not be read.");
		
		// Dump details of the video to standard error
		dump_format(pFormatContext, 0, file.c_str(), false);
		
		// We shall just use the first video stream
		video_stream = -1;
		for(int i=0; i < pFormatContext->nb_streams && video_stream == -1; i++)
		{
			if(pFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_VIDEO)
				video_stream = i; // Found one!
		}
		if(video_stream == -1)
			throw FileOpen(file, "No video stream found.");
		
		// Get the codec context for this video stream
		pCodecContext = &(pFormatContext->streams[video_stream]->codec);
		
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
		
		// Hack to fix wrong frame rates
		if(pCodecContext->frame_rate > 1000 && pCodecContext->frame_rate_base == 1)
			pCodecContext->frame_rate_base = 1000;
		
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
		buffer = new CVD::Rgb<CVD::byte>[my_size.x * my_size.y];
	
		// Assign this buffer to image planes in pFrameRGB
		avpicture_fill((AVPicture *)pFrameRGB, reinterpret_cast<uint8_t*>(buffer), PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height);
	
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
		
		if(buffer != 0)
			delete[] buffer;
		
		// Now re-throw
		throw;
	}
}

//
// DESTRUCTOR
//
VideoFileBuffer::~VideoFileBuffer()
{
    delete [] buffer;
    av_free(pFrameRGB);
    av_free(pFrame);
    avcodec_close(pCodecContext);
    av_close_input_file(pFormatContext);
}


//
// READ NEXT FRAME
//
bool VideoFileBuffer::read_next_frame()
{
    AVPacket packet;
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
				frame_time = frame_time + 1.0 / frames_per_second();
			
			// Decode video frame
			int got_picture;
			if(avcodec_decode_video(pCodecContext, pFrame, &got_picture, 
				packet.data, packet.size) == -1)
			{
				throw BadDecode(frame_time);
			}
	
			// Did we get a video frame?
			if(got_picture)
			{
				// Convert the image from its native format to RGB
				img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
					(AVPicture*)pFrame, pCodecContext->pix_fmt, 
					pCodecContext->width, pCodecContext->height);
				
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
inline VideoFileFrame* VideoFileBuffer::get_frame()
{
	if(!frame_pending())
		throw EndOfFile();

// 	Don't use - pCC->frame_number doesn't reset after a seek!
//  Instead, we ask the packet its time when we decode it
//	double time = start_time + pCodecContext->frame_number * pCodecContext->frame_rate_base / static_cast<double>(pCodecContext->frame_rate);
	VideoFileFrame* vf = new VideoFileFrame(frame_time, buffer, my_size);

	if(!read_next_frame())
	{
		switch(end_of_buffer_behaviour)
		{
			case VideoBufferFlags::RepeatLastFrame:
				// Just do nothing--last frame will still be there
				break;
			
			case VideoBufferFlags::UnsetPending:
				frame_ready = false;
			   break;
			
			case VideoBufferFlags::Loop:
				seek_to(start_time);
				break;
		}
	}
	
	return vf;	
}

//
// PUT FRAME
//
inline void VideoFileBuffer::put_frame(VideoFrame<Rgb<byte> >* f)
{
	delete dynamic_cast<VideoFileFrame*>(f);
}

//
// SEEK TO
//
inline void VideoFileBuffer::seek_to(double t)
{
	if(av_seek_frame(pFormatContext, -1, static_cast<int64_t>(t*AV_TIME_BASE+0.5)) < 0)
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
		pCodecContext = &(pFormatContext->streams[video_stream]->codec);
		
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
		
		// Hack to fix wrong frame rates
		if(pCodecContext->frame_rate > 1000 && pCodecContext->frame_rate_base == 1)
			pCodecContext->frame_rate_base = 1000;

		start_time = 0;
		frame_ready = true;
		
		// REOPENED FILE OK
		// Now read frames until we get to the time we want
		
		int frames = static_cast<int>((t / frames_per_second() + 0.5));
		for(int i = 0; i < frames; i++)
		{
			read_next_frame();
		}
	}
	
	if(!read_next_frame())
			throw BadSeek(t);
}


} // namespace CVD
