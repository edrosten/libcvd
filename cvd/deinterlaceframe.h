#ifndef __DEINTERLACE_FRAME_H__
#define __DEINTERLACE_FRAME_H__

#include <cvd/videoframe.h>

namespace CVD
{
	template<typename T> class DeinterlaceBuffer;
	
	//
	// DEINTERLACEFRAME 
	//
	template<typename T> 
	class DeinterlaceFrame: public VideoFrame<T>
	{
		friend class DeinterlaceBuffer<T>;
		
		protected:
			~DeinterlaceFrame()
			{
			}

			DeinterlaceFrame(double time, T* data, const ImageRef& size) :
			   VideoFrame<T>(time, data, size)
			{
			}	

			// Access the original (deinterlaced) frame
			// This is const since the frame might still be needed!
			const VideoFrame<T>* full_frame() {return real_frame;}
			
		public:
			VideoFrame<T>* real_frame;
	};
}


#endif
