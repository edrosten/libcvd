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

template <class T>
class VideoFrame : public BasicImage<T> 
{
	public:
		VideoFrame(double t, T* data, const ImageRef& size) 
		:BasicImage<T>(data, size),my_timestamp(t)
		{
		}

		VideoFrame(double t, const BasicImage<T>& im)
		:BasicImage<T>(im),my_timestamp(t)
		{
		}

		double timestamp() 
		{
			return my_timestamp;
		}
	protected:
		//Can't destruct these. The videobuffer normally has to do
		//special stuff
		virtual ~VideoFrame()
		{
		}

	private:
	  double my_timestamp;  // no of seconds since boot first frame
};

}

#endif
