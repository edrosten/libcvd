#include <cvd/Linux/v4l2buffer.h>

using namespace CVD;

typedef unsigned char pix;
#define DATA_TYPE GL_LUMINANCE

VideoBuffer<pix>* get_vbuf()
{
	return new V4L2Buffer("/dev/video0", 0, V4L2BBMsleep);
}

#include "test/videoprog.cxx"
