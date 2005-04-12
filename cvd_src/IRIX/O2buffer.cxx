#include <cvd/IRIX/O2buffer.h>
#include <iostream>
#include <cvd/IRIX/sgi-video.h>
#include <cvd/videoframe.h>

using namespace CVD;
using namespace std;

CVD::O2Buffer::O2Buffer () :
  m_pFrame (NULL)
{
  m_pVid = new video_buffer::sgi_video (1);
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
  unsigned char * f = m_pVid->next_frame ();
  return (m_pVid->frame_number () > m_currentFrameNumber);
}

double CVD::O2buffer::frame_rate()
{
	return m_pVid->frame_rate();
}

