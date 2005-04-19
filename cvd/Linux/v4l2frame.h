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

/// A frame from a V4L2Buffer
/// This is an 8-bit greyscale video frame
/// @ingroup gVideoFrame	
class V4L2Frame : public VideoFrame<unsigned char>
{
	friend class V4L2Buffer;

	private:
		/// (Used internally) Construct a video frame
		/// @param t The timestamp
		/// @param size The image size
		/// @param index The index
		/// @param data The image data
		/// @param f The field
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
		// should probably be in videoframe....
		int field;  ///< The field

};

}

#endif
