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
		class RawVideoFileBufferPIMPL;
	}

	/// A frame from a VideoFileBuffer.
	/// @ingroup gVideoFrame	
	/// @param T The pixel type of the video frames. Currently only <code>CVD::Rgb<CVD::byte> ></code> and 
	/// <code>CVD::byte></code> are supported.
	template<class T> 
	class VideoFileFrame: public CVD::LocalVideoFrame<T>
	{
		friend class VFB::RawVideoFileBufferPIMPL;

	        public:
		        inline void delete_self() {  delete this; }
		protected:
			~VideoFileFrame()
			{
			}

			/// Construct a video frame from an Image and a timestamp
			/// @param time The timestamp of this frame
			/// @param local The Image to use for this frame
			VideoFileFrame(double time, CVD::Image<T>& local)
			:LocalVideoFrame<T>(time, local)
			{
			}	

		private:
	};
}


#endif
