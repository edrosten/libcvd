#ifndef CVD_VIDEOBUFFER_H
#define CVD_VIDEOBUFFER_H

#include <cvd/videoframe.h>
#include <cvd/exceptions.h>

namespace CVD {

template <class T> class VideoBuffer 
{
	public:
		virtual ~VideoBuffer(){}

		virtual ImageRef size()=0;
		virtual VideoFrame<T>* get_frame()=0;        	// blocks until frame ready
		virtual void put_frame(VideoFrame<T>* f)=0;  	// user is finished using f
		virtual bool frame_pending()=0;             	// checks to see if frame ready
		virtual void seek_to(double t)					// in seconds
		{}
};

namespace Exceptions
{
	namespace VideoBuffer
	{
		struct All: public CVD::Exceptions::All
		{
		};

		struct BadPutFrame: public All
		{
			BadPutFrame();
		};
	}
}



}

#endif
