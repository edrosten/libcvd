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
// -*- c++ -*-
#ifndef __DVFRAME_H
#define __DVFRAME_H

#include <cvd/videoframe.h>
#include <cvd/byte.h>

namespace CVD {

/// A frame from a Firewire (IEEE 1394) camera via DVBuffer2.
/// Frames are 8-bit greyscale, using the byte type.
/// @ingroup gVideoFrame	
class DVFrame : public VideoFrame<byte> {
public:
	/// (Used internally) Construct a DVFrame.
	/// @param s The image size
	/// @param t The time of this frame
	/// @param buff The buffer number
	/// @param dptr The image data
  DVFrame(ImageRef s, timeval t, int buff, byte* dptr):
    VideoFrame<byte>(t.tv_sec + 0.000001*t.tv_usec, dptr, s)
  {
    //my_size = s;
    my_buffer = buff;
    //my_data = dptr;
  }

  ~DVFrame(){my_data=0;}

  int my_buffer; ///< The buffer number
};

}

#endif
