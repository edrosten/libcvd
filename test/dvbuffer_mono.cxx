#include <cvd/Linux/dvbuffer.h>

using namespace CVD;

typedef  unsigned char pix;
#define  DATA_TYPE GL_LUMINANCE

VideoBuffer<pix>* get_vbuf()
{
	return new DVBuffer(0, 3);
}

#include "test/videoprog.cxx"
