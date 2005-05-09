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
#ifndef O2_BUFFER_H
#define O2_BUFFER_H


#include <cvd/IRIX/O2videoframe.h>
#include <cvd/IRIX/sgi-video.h>
#include <cvd/videobuffer.h>
#include <cvd/rgb8.h>

namespace CVD
{

///	A video buffer to play live video on an %SGI O2.
/// Provides 32-bit colour frames of type CVD::O2VideoFrame.
/// @ingroup gVideoBuffer
class O2Buffer : public VideoBuffer<Rgb8>
{
 public:		
 	/// Construct an O2Buffer
    O2Buffer ();
    ~O2Buffer ();
    
    virtual ImageRef size ();
    virtual O2VideoFrame *get_frame ();
    virtual void put_frame (VideoFrame<Rgb8> *f);
    virtual bool frame_pending ();
	virtual double frame_rate();

 private:
    ImageRef m_frameSize;
    SGI::RawSGIVideo *m_pVid;
    O2VideoFrame *m_pFrame;
    long m_currentFrameNumber;
};

} // namespace

#endif
