//-*- c++ -*-
#ifndef O2_VIDEO_FRAME_H
#define O2_VIDEO_FRAME_H

#include <cvd/videoframe.h>
#include <cvd/rgb8.h>

namespace CVD
{

class O2VideoFrame : public VideoFrame<Rgb8>
{
 public:
    O2VideoFrame (ImageRef &size, Rgb8 *pData, int frameNumber, double timeStamp)
	:VideoFrame<Rgb8>(timeStamp,pData, size),m_frameNumber(frameNumber)
	{}

    long frameNumber ()
	{
		return m_frameNumber;	
	}
 private:
    long m_frameNumber;
};

} // namespace

#endif
