#include <cvd/Linux/dvbuffer.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>

using namespace CVD;

typedef  Rgb<byte> pix;
#define  DATA_TYPE GL_RGB

VideoBuffer<pix>* get_vbuf()
{
	return new DVBuffer2<pix>(0, 3);
}

#include "test/videoprog.cxx"
