// Paul Smith 1 March 2005
// Uses ffmpeg libraries to play most types of video file

#ifndef CVD_VIDEOFILEBUFFER_FRAME_H
#define CVD_VIDEOFILEBUFFER_FRAME_H

#include <cvd/localvideoframe.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>

struct AVCodecContext;

namespace CVD
{
	namespace VFB
	{
		class RawVideoFileBuffer;
	}
	
	template<class C> class VideoFileFrame: public CVD::LocalVideoFrame<C>
	{
		friend class VFB::RawVideoFileBuffer;

		protected:
			~VideoFileFrame()
			{
			}

			VideoFileFrame(double time, CVD::Image<C>& local)
			:LocalVideoFrame<C>(time, local)
			{
			}	

		private:
	};
}


#endif
