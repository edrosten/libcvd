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
/*
 *  qtframe.h
 *  libcvd
 */

#ifndef CVD_QTFRAME_H
#define CVD_QTFRAME_H

#include <cvd/videoframe.h>

namespace CVD {
	
	template <class T> class QTBuffer;
	
	/// A frame from a QTBuffer
	/// @param T the pixel type of the buffer
	/// @ingroup gVideoFrame
	template <class T> class QTFrame : public VideoFrame<T> {
		friend class QTBuffer<T>;
		
	private:
		/// (Used internally) Construct a video frame
		/// @param t The timestamp
		/// @param size The image size
		/// @param data The image data
        QTFrame(double t, T * data, const ImageRef& size) : VideoFrame<T>(t, data, size, VideoFrameFlags::Progressive) 
		{}
		
        ~QTFrame() {
            this->my_data = NULL;
		}
	};
}

#endif
