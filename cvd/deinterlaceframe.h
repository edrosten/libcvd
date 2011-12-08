/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef __DEINTERLACE_FRAME_H__
#define __DEINTERLACE_FRAME_H__

#include <cvd/localvideoframe.h>

namespace CVD
{
	template<typename T> class DeinterlaceBuffer;

	/// A frame from a DeinterlaceBuffer, representing one field from an
	/// interlaced frame.
	/// If the buffer is extracting both fields from the video frames, the
	/// time of the first field is reported as being the time of the
	/// original frame, while the time of the second field will be 
	/// 1/frame_rate() further on.
	/// @param T The pixel type of the original video buffer
	/// @ingroup gVideoFrame
	template<typename T> 
	class DeinterlaceFrame: public LocalVideoFrame<T>
	{
		friend class DeinterlaceBuffer<T>;
		
		public:
			/// Access the original (interlaced) frame
			const VideoFrame<T>* full_frame() {return real_frame;}

		private:
			~DeinterlaceFrame()
			{
			}

			DeinterlaceFrame(double time, Image<T>& im)
			   :LocalVideoFrame<T>(time, im)
			{
			}	
			
		private:
			VideoFrame<T>* real_frame;
	};

}

#endif

