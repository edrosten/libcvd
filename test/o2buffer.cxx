#include <cvd/IRIX/O2buffer.h>
#include <cvd/rgb8.h>

using namespace CVD;

typedef Rgb8 pix;
#define DATA_TYPE GL_RGBA

VideoBuffer<pix>* get_vbuf()
{
	return new O2Buffer();
}

#include "test/videoprog.cxx"
