//-*- c++ -*-

//////////////////////////////////////////////////////////////////////
//                                                                  //
//   v4l2Frame - An image with a timestamp and an index             //
//                                                                  //
//   C Kemp Nov 2002                                                //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifndef __CVD_V4L2FRAME_H
#define __CVD_V4L2FRAME_H

#include <cvd/videoframe.h>
#include <linux/videodev.h>

namespace CVD {

class V4L2Frame : public VideoFrame<unsigned char>
{
	friend class V4L2Buffer;

	private:

		V4L2Frame(double t, const ImageRef& size, int index, unsigned char *data, int f) 
		: VideoFrame<unsigned char>(t, data, size),my_index(index),field(f)
		{
		}
		

		//Only 2.6 needs this, but it does no harm in 2.4
		struct v4l2_buffer* m_buf;


	  int my_index;

	  ~V4L2Frame() 
	  {}

	public:
		int field;  // should probably be in videoframe....

};

}

#endif
