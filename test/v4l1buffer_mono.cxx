#include <cvd/Linux/v4l1buffer.h>

using namespace CVD;

typedef  unsigned char pix;
#define  DATA_TYPE GL_LUMINANCE

VideoBuffer<pix>* get_vbuf()
{
	return new V4L1Buffer<pix>("/dev/video0");
}

#include "test/videoprog.cxx"
