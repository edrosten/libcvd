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
//-*- c++ -*-

//////////////////////////////////////////////////////////////////////
//                                                                  //
//   v4l2Frame - An image with a timestamp and an index             //
//                                                                  //
//   C Kemp Nov 2002                                                //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifndef CVD_V4L2FRAME_H
#define CVD_V4L2FRAME_H

#include <cvd/videoframe.h>
#include <linux/videodev.h>

namespace CVD {

/// A frame from a V4L2Buffer
/// This is an 8-bit greyscale video frame
/// @ingroup gVideoFrame	
class V4L2Frame_Base : public VideoFrame<unsigned char>
{
	friend class V4L2Buffer_Base;

	private:
		/// (Used internally) Construct a video frame
		/// @param t The timestamp
		/// @param size The image size
		/// @param index The index
		/// @param data The image data
		/// @param f The field
		V4L2Frame_Base(double t, const ImageRef& size, int index, unsigned char *data, VideoFrameFlags::FieldType f) 
		: VideoFrame<unsigned char>(t, data, size, f),my_index(index)
		{
		}
		

		//Only 2.6 needs this, but it does no harm in 2.4
		struct v4l2_buffer* m_buf;


	  int my_index;

	  ~V4L2Frame_Base() 
	  {}

	public:

};

template <class T>
class V4L2FrameT : public V4L2Frame_Base {};

typedef V4L2FrameT<unsigned char> V4L2Frame;


}

#endif
