//////////////////////////////////////////////////////////////////////
//                                                                  //
//   VideoFrame - An image with a timestamp, like from a video      //
//                source                                            //
//                                                                  //
//   Tom Drummond     3 April 2002                                  //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifndef CVD_VIDEOFRAME_H
#define CVD_VIDEOFRAME_H

#include <cvd/image.h>

namespace CVD {

/// A frame from a VideoBuffer.
/// @param T The pixel type of the video frames
/// @ingroup gVideoFrame
template <class T>
class VideoFrame : public BasicImage<T> 
{
	public:
		/// (Used internally) Construct a VideoFrame around a block of memory. The
		/// memory is not managed by the VideoFrame so must me managed seperately
		/// @param t The time (in seconds) of this frame
		/// @param data The image data for this frame
		/// @param size The size of this video frame
		VideoFrame(double t, T* data, const ImageRef& size) 
		:BasicImage<T>(data, size),my_timestamp(t)
		{
		}

		/// (Used internally) Construct a VideoFrame from a BasicImage
		/// @param t The time (in seconds) of this frame
		/// @param im The image data for this frame. BasicImages do not manage their own
		/// memory, so this must be managed externally
		VideoFrame(double t, const BasicImage<T>& im)
		:BasicImage<T>(im),my_timestamp(t)
		{
		}

		/// What is the time (since boot) of this frame?
		double timestamp() const
		{
			return my_timestamp;
		}
	protected:
		/// We don't usually <code>delete</code> video frames. Some special destruction is usually needed.
		virtual ~VideoFrame()
		{
		}

		double my_timestamp;  ///< No of seconds since boot of this frame
};

}


#endif
