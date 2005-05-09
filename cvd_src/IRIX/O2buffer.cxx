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
#include <cvd/IRIX/O2buffer.h>
#include <iostream>
#include <cvd/IRIX/sgi-video.h>
#include <cvd/videoframe.h>

using namespace CVD;
using namespace std;

CVD::O2Buffer::O2Buffer () :
  m_pFrame (NULL)
{
  m_pVid = new SGI::RawSGIVideo(1);
  m_pVid->start ();
  m_currentFrameNumber = 0;

  // Get the size 
  ImageRef imageRef (m_pVid->x_size (), m_pVid->y_size ());
  m_frameSize = imageRef;
  
  cerr << "O2 buffer frame size: (" << m_frameSize.x << ", " << m_frameSize.y
       << ")." << endl;;
}

CVD::O2Buffer::~O2Buffer ()
{
  m_pVid->stop ();
  m_pFrame = NULL;
}

ImageRef CVD::O2Buffer::size ()
{
  return m_frameSize;
}

O2VideoFrame *CVD::O2Buffer::get_frame ()
{
  // wait for a frame
  while (!frame_pending ());

  delete m_pFrame;
  m_pFrame = new O2VideoFrame (m_frameSize, (Rgb8*)m_pVid->current_frame (),
				  m_currentFrameNumber, 0);

  m_currentFrameNumber = m_pFrame->frameNumber ();
  return m_pFrame;
}

void CVD::O2Buffer::put_frame (VideoFrame<Rgb8> *f)
{
  // no need to put frames as get_frame destroys frame objects
}

bool CVD::O2Buffer::frame_pending ()
{
  // there is a frame pending if the sgi video frame number is greater than
  // the current O2 videoframe's number

  // next_frame does all the sgi video work
  m_pVid->next_frame ();
  return (m_pVid->frame_number () > m_currentFrameNumber);
}

double CVD::O2Buffer::frame_rate()
{
	return m_pVid->frame_rate();
}

