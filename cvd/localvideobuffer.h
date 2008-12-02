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
#ifndef CVD_VIDEOBUFFER_LOCAL_H
#define CVD_VIDEOBUFFER_LOCAL_H

#include <cvd/localvideoframe.h>
#include <cvd/videobuffer.h>
#include <cvd/exceptions.h>

namespace CVD {

/// Base class for a VideoBuffer which manages its own memory for each VideoFrame that 
/// it provides. (In contrast to most buffers, where the memory is managed 
/// by the video hardware.)
/// @param T The pixel type of the video frames
/// @ingroup gVideoBuffer
template <class T> 
class LocalVideoBuffer: public  CVD::VideoBuffer<T>
{
	public:
		LocalVideoBuffer(typename VideoBufferType::Type t)
		:VideoBuffer<T>(t)
		{}

		virtual ~LocalVideoBuffer(){}

		virtual ImageRef size()=0;
		virtual LocalVideoFrame<T>* get_frame()=0;     	// blocks until frame ready
		virtual void put_frame(VideoFrame<T>* f)=0;  	// user is finished using f
		virtual bool frame_pending()=0;             	// checks to see if frame ready
		virtual void seek_to(double)					// in seconds
		{}
};

}

#endif
