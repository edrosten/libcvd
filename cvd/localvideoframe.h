/*******************************************************************************
                                                                  
   LocalVideoFrame - A video frame for when the data is local snd should be
   					 managed by the program instead of the system. Uses Image
					 to manage the memory. Programs which will only ever use 
					 these can be optimized by using the image() methos. These
					 can be deleted sensibly, but it is not currently allowed, 
					 to make the interface more consistent.
                                                               
   E. Rosten	   - 18 March 2005
                                                             
*******************************************************************************/


#ifndef CVD_LOCALVIDEOFRAME_FRAME_H
#define CVD_LOCALVIDEOFRAME_FRAME_H

#include <cvd/videoframe.h>
#include <string>

namespace CVD
{
	template<class T> class LocalVideoFrame: public VideoFrame<T>
	{
		public:		
			const std::string& name() {return *frame_name;};


		protected:

			virtual ~LocalVideoFrame()
			{
			}

			LocalVideoFrame(double time, CVD::Image<T>& local_image)
			:VideoFrame<T>(time, local_image.data(), local_image.size()),
			 im(local_image)
			{
			}	

			const Image<T>& image()
			{
				return im;
			}

			
		private:
			CVD::Image<T>		  im;
	};
}


#endif
