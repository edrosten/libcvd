#include <cvd/diskbuffer2.h>
#include <cvd/byte.h>

using namespace CVD;
using namespace std;

typedef  Rgb<byte> pix;
#define  DATA_TYPE GL_RGB

VideoBuffer<pix>* get_vbuf()
{
	return new DiskBuffer2<pix>(globlist("/home/cabinet1/er258/videos/corridor/18/*.pgm"),  25.);
}

#include "test/videoprog.cxx"
