#include <cvd/colourspacebuffer.h>
#include <cvd/Linux/v4l1buffer.h>

using namespace CVD;
typedef  Rgb<byte> pix;

VideoBuffer<pix>* get_vbuf()
{
	return new ColourspaceBuffer_managed<pix,bayer>(new V4L1Buffer<bayer>("/dev/video0"));
}

#include "test/videoprog.cxx"
