//-*- c++ -*-
#ifndef O2_BUFFER_H
#define O2_BUFFER_H


#include <cvd/IRIX/O2videoframe.h>
#include <cvd/IRIX/sgi-video.h>
#include <cvd/videobuffer.h>
#include <cvd/rgb8.h>

namespace CVD
{

class O2Buffer : public VideoBuffer<Rgb8>
{
 public:		
    O2Buffer ();
    ~O2Buffer ();
    
    virtual ImageRef size ();
    virtual O2VideoFrame *get_frame ();
    virtual void put_frame (VideoFrame<Rgb8> *f);
    virtual bool frame_pending ();
	virtual double frame_rate();

 private:
    ImageRef m_frameSize;
    video_buffer::sgi_video *m_pVid;
    O2VideoFrame *m_pFrame;
    long m_currentFrameNumber;
};

} // namespace

#endif
