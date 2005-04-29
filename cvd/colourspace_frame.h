#ifndef CVD_COLOURSPACE_FRAME_H
#define CVD_COLOURSPACE_FRAME_H

#include <cvd/localvideoframe.h>
#include <string>

namespace CVD
{
	template<class To, class From> class ColourspaceBuffer;

	/// A frame from a ColourspaceBuffer. Can be treated as a VideoFrame
	template<typename T> 
	class ColourspaceFrame : public CVD::LocalVideoFrame<T>
	{
		/// Allow ColourspaceBuffer to manage frames.
		friend template<class From> CVD::ColourspaceBuffer<To, From>;

		protected:
			~ColourspaceFrame()
			{
			}

			ColourspaceFrame(double time, CVD::Image<T>& converted)
			:LocalVideoFrame<T>(time, converted)
			{
			}	
	};
}


#endif
