//-*- c++ -*-
#ifndef O2_VIDEO_FRAME_H
#define O2_VIDEO_FRAME_H

#include <cvd/videoframe.h>
#include <cvd/rgb8.h>

namespace CVD
{

/// A frame from an O2Buffer.
/// Frames are 32-bit colour, using the Rgb8 type.
/// @ingroup gVideoFrame	
class O2VideoFrame : public VideoFrame<Rgb8>
{
 public:
 	/// (Used internally) Construct a video frame
	/// @param size The image size
	/// @param pData The raw image data
	/// @param frameNumber The frame number
	/// @param timeStamp The time stamp
    O2VideoFrame (ImageRef &size, Rgb8 *pData, int frameNumber, double timeStamp)
	:VideoFrame<Rgb8>(timeStamp,pData, size),m_frameNumber(frameNumber)
	{}

	/// What is the frame number of this frame?
    long frameNumber ()
	{
		return m_frameNumber;	
	}
 private:
    long m_frameNumber;
};

} // namespace

#endif
