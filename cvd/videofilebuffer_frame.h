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
	class VideoFileBuffer;
	
	class VideoFileFrame: public CVD::LocalVideoFrame<CVD::Rgb<CVD::byte> >
	{
		friend class VideoFileBuffer;

		protected:
			~VideoFileFrame()
			{
			}

			VideoFileFrame(double time, CVD::Image<CVD::Rgb<CVD::byte> >& local)
			:LocalVideoFrame<CVD::Rgb<CVD::byte> >(time, local)
			{
			}	

		private:
	};
}


#endif
