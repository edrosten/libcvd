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
//   V4L1Frame - An image with a timestamp and an index             //
//                                                                  //
//   G Reitmayr APRIL 2005                                          //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifndef __CVD_V4L1FRAME_H
#define __CVD_V4L1FRAME_H

#include <cvd/videoframe.h>


#ifdef CVD_INTERNAL_HAVE_STRANGE_V4L2
	#include <videodevx/videodev.h>
#else
	#include <linux/videodev.h>
#endif



namespace CVD {


template <class T> class V4L1Buffer;

/// A frame from a V4L1Buffer
/// @param T the pixel type of the buffer
/// @ingroup gVideoFrame
template <class T>
class V4L1Frame : public VideoFrame<T>
{
	friend class V4L1Buffer<T>;

private:
	/// (Used internally) Construct a video frame
	/// @param t The timestamp
	/// @param size The image size
	/// @param index The index
	/// @param data The image data
        V4L1Frame(double t, T * data, const ImageRef& size) : VideoFrame<T>(t, data, size)
        {
	}

        ~V4L1Frame()
        {
            this->my_data = NULL;
        }
};

}

#endif
