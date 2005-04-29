#ifndef CVD_VIDEOBUFFER_H
#define CVD_VIDEOBUFFER_H

#include <cvd/videoframe.h>
#include <cvd/exceptions.h>

namespace CVD {

/// Base class for objects which provide a video stream. A video 
/// stream is a sequence of video frames (derived from VideoFrame).
/// @param T The pixel type of the video frames
/// @ingroup gVideoBuffer
template <class T> 
class VideoBuffer 
{
	public:
		virtual ~VideoBuffer(){}

		/// The size of the VideoFrames returned by this buffer
		virtual ImageRef size()=0;
		/// Returns the next frame from the buffer. This function blocks until a frame is ready.
		virtual VideoFrame<T>* get_frame()=0;        	
		/// Tell the buffer that you are finished with this frame. Typically the VideoBuffer then destroys the frame.
		/// \param f The frame that you are finished with.
		virtual void put_frame(VideoFrame<T>* f)=0;
		/// Is there a frame waiting in the buffer? This function does not block. 
		virtual bool frame_pending()=0;
		/// What is the (expected) frame rate of this video buffer, in frames per second?		
		virtual double frame_rate()=0;
		/// Go to a particular point in the video buffer (only implemented in buffers of recorded video)
		/// \param t The frame time in seconds
		virtual void seek_to(double)
		{}
};

namespace Exceptions
{
	/// %Exceptions specific to VideoBuffer
	/// @ingroup gException
	namespace VideoBuffer
	{
		/// Base class for all VideoBuffer exceptions
		/// @ingroup gException
		struct All: public CVD::Exceptions::All
		{
		};

		/// The VideoBuffer was unable to successfully complete a VideoBuffer::put_frame() operation
		/// @ingroup gException
		struct BadPutFrame: public All
		{
			BadPutFrame();
		};
	}
}



}

#endif
