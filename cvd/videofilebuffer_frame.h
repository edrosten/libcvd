// Paul Smith 1 March 2005
// Uses ffmpeg libraries to play most types of video file

#ifndef __VIDEOFILEFRAME_H__
#define __VIDEOFILEFRAME_H__

#include <cvd/videoframe.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>

struct AVCodecContext;

namespace CVD
{
	class VideoFileBuffer;
	
	class VideoFileFrame: public VideoFrame<CVD::Rgb<CVD::byte> >
	{
		friend class VideoFileBuffer;

		protected:
			~VideoFileFrame()
			{
			}

			VideoFileFrame(double time, CVD::Rgb<CVD::byte>* data, const ImageRef& size) :
				VideoFrame<CVD::Rgb<CVD::byte> >(time, data, size)
			{
			}	

			
		private:
	};
}


#endif
