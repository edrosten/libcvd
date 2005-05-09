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
#ifndef O2_VIDEO_FRAME_H
#define O2_VIDEO_FRAME_H

#include <cvd/videoframe.h>
#include <cvd/rgb8.h>

namespace CVD
{

/// A frame from an O2Buffer.
/// Frames are 32-bit colour, using the Rgb8 type.
/// @ingroup gVideoFrame	
class O2VideoFrame : public VideoFrame<Rgb8>
{
 public:
 	/// (Used internally) Construct a video frame
	/// @param size The image size
	/// @param pData The raw image data
	/// @param frameNumber The frame number
	/// @param timeStamp The time stamp
    O2VideoFrame (ImageRef &size, Rgb8 *pData, int frameNumber, double timeStamp)
	:VideoFrame<Rgb8>(timeStamp,pData, size),m_frameNumber(frameNumber)
	{}

	/// What is the frame number of this frame?
    long frameNumber ()
	{
		return m_frameNumber;	
	}
 private:
    long m_frameNumber;
};

} // namespace

#endif
